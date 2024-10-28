// #define SPDLOG_FMT_EXTERNAL
// #define SPDLOG_USE_STD_FORMAT
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>

#include "ct_sim.h"

using std::size_t;
namespace fs = std::filesystem;

auto main(int argc, char* argv[]) -> int {
  if (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Running ct_ray_sim in debug mode.");
  }
  argparse::ArgumentParser program("ct_ray_sim");

  program.add_argument("--inputPath").help("Path to the input image file.").required();

  program.add_argument("--outputPath")
    .help("Path to the output directory where projections will be saved.")
    .default_value(std::string("output"));

  program.add_argument("--angles")
    .help("Number of angles for simulation.")
    .default_value(512)
    .scan<'i', size_t>();

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::exception& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  // Retrieve CLI arguments
  auto inputPath = program.get<std::string>("--inputPath");
  auto outputPath = program.get<std::string>("--outputPath");
  auto numAngles = program.get<size_t>("--angles");

  spdlog::info(
    "Starting CT simulation with inputPath: {}, outputPath: {}, angles: {}",
    inputPath,
    outputPath,
    numAngles
  );

  // Run CT simulation
  auto sim = CtSim(inputPath, numAngles);
  sim.run();
  sim.saveProjectionImage(outputPath);

  spdlog::info("CT simulation completed successfully.");
  return 0;
}
