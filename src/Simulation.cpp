#include "Simulation.hpp"

#include <spdlog/spdlog.h>

using namespace glm;
using std::size_t;

Simulation::Simulation(const DensityMap& densityMap)
    : m_densityMap(densityMap),
      m_rayTracer(m_densityMap) { }

Simulation::Simulation(DensityMap&& densityMap)
    : m_densityMap(std::move(densityMap)),
      m_rayTracer(m_densityMap) { }

SimulationResult Simulation::simulateCT(const std::size_t numAngles) const {
    spdlog::info("Starting CT simulation with {} angles.", numAngles);

    const auto imageSize = m_densityMap.getSize();
    auto projections = cv::Mat(imageSize, numAngles, CV_64F, cv::Scalar(0));

    for (size_t i = 0; i < numAngles; ++i) {
        const auto phi = radians(static_cast<double>(i) * (360.0 / numAngles));
        simulateProjectionForAngle(phi).copyTo(projections.col(i));
    }

    filterProjections(projections);

    auto image = backProject(projections);
    return SimulationResult(image, projections);
}

cv::Mat Simulation::simulateProjectionForAngle(const double phi) const {
    spdlog::debug("Simulating projection for angle: {:.2f} degrees", degrees(phi));
    const auto rays = m_rayTracer.setupRays(phi, m_densityMap.getSize());
    auto projection = cv::Mat(m_densityMap.getSize(), 1, CV_64F, cv::Scalar(0));

    for (size_t i = 0; i < rays.size(); ++i)
        projection.at<double>(i, 0) = m_rayTracer.traceRay(rays[i]);

    return projection;
}

cv::Mat& Simulation::filterProjections(cv::Mat& projections) const {
    cv::normalize(projections, projections, 0.0, 1.0, cv::NORM_MINMAX);

    return projections;
}

cv::Mat Simulation::backProject(const cv::Mat& projections) const {
    spdlog::info("Starting reconstruction of the image from projections.");

    const auto imageSize = static_cast<int32_t>(m_densityMap.getSize());
    auto reconstructedImage = cv::Mat(imageSize, imageSize, CV_64F, cv::Scalar(0));

    const auto center = static_cast<double>(imageSize) / 2.0;
    const auto numAngles = static_cast<size_t>(projections.cols);

    for (size_t i = 0; i < numAngles; i++) {
        const auto phi = radians(static_cast<double>(i) * (360.0 / numAngles));
        const auto cosAngle = cos(phi);
        const auto sinAngle = sin(phi);

        spdlog::debug("Processing angle {} ({} degrees)", i, degrees(phi));

        for (int32_t y = 0; y < imageSize; y++) {
            for (int32_t x = 0; x < imageSize; x++) {
                const auto xRel = static_cast<double>(x) - center;
                const auto yRel = static_cast<double>(y) - center;

                const auto t = -xRel * sinAngle + yRel * cosAngle;

                const auto detectorCenter = static_cast<double>(imageSize) / 2.0;
                const auto detectorIndex = t + detectorCenter;

                const auto index0 = static_cast<int32_t>(std::floor(detectorIndex));
                const auto index1 = index0 + 1;
                const auto weight1 = detectorIndex - static_cast<double>(index0);
                const auto weight0 = 1.0 - weight1;

                auto projectionValue = 0.0;

                if (index0 >= 0 && index1 < imageSize) {
                    const auto value0 = projections.at<double>(index0, i);
                    const auto value1 = projections.at<double>(index1, i);
                    projectionValue = weight0 * value0 + weight1 * value1;
                }
                else if (index0 >= 0 && index0 < imageSize) {
                    projectionValue = projections.at<double>(index0, i);
                }
                else if (index1 >= 0 && index1 < imageSize) {
                    projectionValue = projections.at<double>(index1, i);
                }
                else {
                    continue;
                }

                reconstructedImage.at<double>(y, x) += projectionValue;
            }
        }
    }

    return reconstructedImage;
}
