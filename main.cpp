#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <thinks/testFastMarchingMethod.hpp>


namespace {

template <typename R, typename T> inline
R clamp(T const min, T const max, T const value)
{
  return static_cast<R>(std::min<T>(max, std::max<T>(min, value)));
}


template<typename T>
struct Pixel
{
  typedef T ChannelType;
  Pixel() : r(0), g(0), b(0) {}
  Pixel(T const x) : r(x), g(x), b(x) {}
  Pixel(T const _r, T const _g, T const _b) : r(_r), g(_g), b(_b) {}
  T r;
  T g;
  T b;
};

typedef Pixel<std::uint8_t> Pixel8;

void writePpm(std::string const& filename,
              std::size_t const width, std::size_t const height,
              std::vector<Pixel8> const& pixels)
{
  // C-HACK!!
  FILE* fd = fopen(filename.c_str(), "wb");
  (void) fprintf(fd, "P6\n%d %d\n255\n", width, height);
  (void) fwrite((const void*)pixels.data(), sizeof(Pixel8),
                width * height, fd);
  (void) fflush(fd);
  fclose(fd);
}

template<typename T, typename C>
std::vector<Pixel8> pixels(std::vector<T> const& values,
                                        C const converter)
{
  using namespace std;

  auto pixels = vector<Pixel8>(values.size());
  transform(begin(values), end(values), begin(pixels), converter);
  return pixels;
}


template<typename T>
void writeGradMagImages(
  thinks::fmm::test::GradientMagnitudeStats<T, 2> const& grad_mag_stats,
  std::string const& prefix)
{
  using namespace std;

  stringstream ss_input;
  ss_input << prefix << "_input_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_input.str(),
    grad_mag_stats.grid_size[0], grad_mag_stats.grid_size[1],
    pixels(
      grad_mag_stats.input_buffer,
      [](T const d) {
        if (isnan(d)) {
          // A pleasant shade of green for background, i.e. non-input pixels.
          return Pixel8(
            Pixel8::ChannelType(127),
            Pixel8::ChannelType(200),
            Pixel8::ChannelType(127));
        }
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));

  stringstream ss_distance;
  ss_distance << prefix << "_distance_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_distance.str(),
    grad_mag_stats.grid_size[0], grad_mag_stats.grid_size[1],
    pixels(
      grad_mag_stats.distance_buffer,
      [](T const d) {
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));

  stringstream ss_grad_mag;
  ss_grad_mag << prefix << "_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_grad_mag.str(),
    grad_mag_stats.grid_size[0], grad_mag_stats.grid_size[1],
    pixels(
      grad_mag_stats.grad_buffer,
      [](array<T, 2> const& v) {
        if (isnan(v[0]) || isnan(v[1])) {
          return Pixel8(
            Pixel8::ChannelType(255),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
        }
        auto const mag = sqrt(v[0] * v[0] + v[1] * v[1]);
        return Pixel8(
          clamp<Pixel8::ChannelType>(
            T(0),
            T(numeric_limits<Pixel8::ChannelType>::max()),
            numeric_limits<Pixel8::ChannelType>::max() * mag));
      }));
}

template<typename T>
void writeDistStatImages(
  thinks::fmm::test::DistanceValueStats<T, 2> const& dist_stats,
  std::string const& prefix)
{
  using namespace std;

  stringstream ss_input;
  ss_input << prefix << "_input_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_input.str(),
    dist_stats.grid_size[0], dist_stats.grid_size[1],
    pixels(
      dist_stats.input_buffer,
      [](T const d) {
        if (isnan(d)) {
          // A pleasant shade of green for background, i.e. non-input pixels.
          return Pixel8(
            Pixel8::ChannelType(127),
            Pixel8::ChannelType(200),
            Pixel8::ChannelType(127));
        }
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));

  stringstream ss_distance;
  ss_distance << prefix << "_distance_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_distance.str(),
    dist_stats.grid_size[0], dist_stats.grid_size[1],
    pixels(
      dist_stats.distance_buffer,
      [](T const d) {
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));

  stringstream ss_gt;
  ss_gt << prefix << "_gt_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_gt.str(),
    dist_stats.grid_size[0], dist_stats.grid_size[1],
    pixels(
      dist_stats.distance_ground_truth_buffer,
      [](T const d) {
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));

  stringstream ss_error;
  ss_error << prefix << "_error_" << typeid(T).name() << ".ppm";
  writePpm(
    ss_error.str(),
    dist_stats.grid_size[0], dist_stats.grid_size[1],
    pixels(
      dist_stats.error_buffer,
      [](T const d) {
        return d < T(0) ?
          Pixel8(
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0),
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * fabs(d))) :
          Pixel8(
            clamp<Pixel8::ChannelType>(
              T(0),
              T(numeric_limits<Pixel8::ChannelType>::max()),
              numeric_limits<Pixel8::ChannelType>::max() * d),
            Pixel8::ChannelType(0),
            Pixel8::ChannelType(0));
      }));
}

} // namespace


namespace std {

template<typename T, size_t N>
ostream& operator<<(
  ostream& os,
  thinks::fmm::test::GradientMagnitudeStats<T, N> const& grad_mag_stats)
{
  os << "Gradient magnitude stats <" << typeid(T).name() << ", " << N << ">:" << endl
    << "min: " << grad_mag_stats.min << endl
    << "max: " << grad_mag_stats.max << endl
    << "avg: " << grad_mag_stats.avg << endl
    << "std_dev: " << grad_mag_stats.std_dev << endl;
  return os;
}


template<typename T, size_t N>
ostream& operator<<(
  ostream& os,
  thinks::fmm::test::DistanceValueStats<T, N> const& dist_stats)
{
  os << "Distance value stats <" << typeid(T).name() << ", " << N << ">:" << endl
    << "min error: " << dist_stats.min_error << endl
    << "max error: " << dist_stats.max_error << endl
    << "avg error: " << dist_stats.avg_error << endl
    << "std_dev error: " << dist_stats.std_dev_error << endl;
  return os;
}

} // namespace std


int main(int argc, char* argv[])
{
  using namespace std;
  using namespace thinks::fmm::test;

  try {
    typedef uint16_t ChannelType;

    {
      cout << "Unsigned distance" << endl
           << "-----------------" << endl;

      auto const grad_mag_stats2f = UnsignedGradientMagnitudeStats<float, 2>();
      cout << grad_mag_stats2f << endl;
      writeGradMagImages<float>(grad_mag_stats2f, "unsigned_grad_mag");

      auto const grad_mag_stats2d = UnsignedGradientMagnitudeStats<double, 2>();
      cout << grad_mag_stats2d << endl;
      writeGradMagImages<double>(grad_mag_stats2d, "unsigned_grad_mag");

      auto const grad_mag_stats3f = UnsignedGradientMagnitudeStats<float, 3>();
      cout << grad_mag_stats3f << endl;
      auto const grad_mag_stats3d = UnsignedGradientMagnitudeStats<double, 3>();
      cout << grad_mag_stats3d << endl;

      //thinks::fmm::test::testUnsignedGradientMagnitude<float, 4>();
      //thinks::fmm::test::testUnsignedGradientMagnitude<double, 4>();

      auto const dist_stats2f = UnsignedDistanceValueStats<float, 2>();
      cout << dist_stats2f << endl;
      writeDistStatImages<float>(dist_stats2f, "unsigned_dist_stat");

      auto const dist_stats2d = UnsignedDistanceValueStats<double, 2>();
      cout << dist_stats2d << endl;
      writeDistStatImages<double>(dist_stats2d, "unsigned_dist_stat");

      auto const dist_stats3f = UnsignedDistanceValueStats<float, 3>();
      cout << dist_stats3f << endl;
      auto const dist_stats3d = UnsignedDistanceValueStats<double, 3>();
      cout << dist_stats3d << endl;

      //thinks::fmm::test::testUnsignedDistanceValues<float, 4>();
      //thinks::fmm::test::testUnsignedDistanceValues<double, 4>();
    }

    {
      cout << "Signed distance" << endl
           << "-----------------" << endl;

      auto const grad_mag_stats2f = SignedGradientMagnitudeStats<float, 2>();
      cout << grad_mag_stats2f << endl;
      writeGradMagImages<float>(grad_mag_stats2f, "signed_grad_mag");

      auto const grad_mag_stats2d = SignedGradientMagnitudeStats<double, 2>();
      cout << grad_mag_stats2d << endl;
      writeGradMagImages<double>(grad_mag_stats2d, "signed_grad_mag");

      auto const grad_mag_stats3f = SignedGradientMagnitudeStats<float, 3>();
      cout << grad_mag_stats3f << endl;
      auto const grad_mag_stats3d = SignedGradientMagnitudeStats<double, 3>();
      cout << grad_mag_stats3d << endl;
      //thinks::fmm::test::testUnsignedGradientMagnitude<float, 4>();
      //thinks::fmm::test::testUnsignedGradientMagnitude<double, 4>();

      auto const dist_stats2f = SignedDistanceValueStats<float, 2>();
      cout << dist_stats2f << endl;
      writeDistStatImages<float>(dist_stats2f, "signed_dist_stat");

      auto const dist_stats2d = SignedDistanceValueStats<double, 2>();
      cout << dist_stats2d << endl;
      writeDistStatImages<double>(dist_stats2d, "signed_dist_stat");

      auto const dist_stats3f = SignedDistanceValueStats<float, 3>();
      cout << dist_stats3f << endl;
      auto const dist_stats3d = SignedDistanceValueStats<double, 3>();
      cout << dist_stats3d << endl;
      //thinks::fmm::test::testUnsignedDistanceValues<float, 4>();
      //thinks::fmm::test::testUnsignedDistanceValues<double, 4>();
    }
  }
  catch (exception& ex) {
    cerr << ex.what() << endl;
    return 1;
  }

  return 0;
}


