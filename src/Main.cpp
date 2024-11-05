#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <cstdlib>
#include <filesystem>
#include <memory>

#include "PostProcessing.hpp"
#include "Simulation.hpp"

using std::size_t;
namespace fs = std::filesystem;

struct CLIArguments {
    std::string inputPath;
    std::string outputPath;
    size_t angles;

    static CLIArguments parse(int argc, char* argv[]) {
        auto program = argparse::ArgumentParser("ct_ray_sim");

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
            spdlog::error("Error parsing CLI arguments: {}", err.what());
            std::exit(EXIT_FAILURE);
        }

        return { program.get<std::string>("--inputPath"),
                 program.get<std::string>("--outputPath"),
                 program.get<size_t>("--angles") };
    }
};

void setupLogger() {
    if (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG) {
        spdlog::set_level(spdlog::level::debug);
        spdlog::debug("Running ct_ray_sim in debug mode.");
    }
}

int32_t main(int32_t argc, char* argv[]) {
    setupLogger();
    const auto args = CLIArguments::parse(argc, argv);

    spdlog::info(
        "Starting CT simulation with inputPath: {}, outputPath: {}, angles: {}",
        args.inputPath,
        args.outputPath,
        args.angles
    );

    const auto densityMap = DensityMap(args.inputPath);
    const auto sim = Simulation(densityMap);
    const auto res = sim.simulateCT(args.angles);

    if (!fs::exists(args.outputPath)) {
        if (!fs::create_directory(args.outputPath)) {
            spdlog::error("Failed to create output directory: {}", args.outputPath);
            throw std::runtime_error("Failed to create output directory.");
        }
        spdlog::info("Created output directory: {}", args.outputPath);
    }

    PostProcessing(res.getProjections())
        .to8U()
        .saveImage(fs::path(args.outputPath) / "projections.png");

    PostProcessing(res.getImage())
        .normalize()
        .to8U()
        .saveImage(fs::path(args.outputPath) / "reconstructed_image.png");

    spdlog::info("CT simulation completed successfully.");
    return EXIT_SUCCESS;
}