// #define SPDLOG_FMT_EXTERNAL
// #define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

#include "ct_sim.h"

using std::size_t;

auto main() -> int {
  constexpr auto inputPath = "org(1).png";
  constexpr auto outputPath = "output_projection.png";
  constexpr size_t numAngles = 512;

  auto sim = CtSim(inputPath, numAngles);
  sim.run();
  sim.saveProjectionImage(outputPath);

  return 0;
}
