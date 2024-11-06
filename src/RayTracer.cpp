#include "RayTracer.hpp"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <ranges>

using namespace glm;

using std::numeric_limits;
using std::size_t;
using std::vector;

RayTracer::RayTracer(const DensityMap& densityMap) : m_densityMap(densityMap) { }

RayTracer::RayTracer(DensityMap&& densityMap) : m_densityMap(std::move(densityMap)) { }

vector<Ray> RayTracer::setupRays(const double phi, const size_t numRays) const {
    spdlog::debug("Setting up rays for angle: {:.2f} ({} rays)", phi, numRays);

    const auto imageSize = m_densityMap.getSize();
    const auto radius = imageSize / 2.0;
    const auto center = dvec2(1.0, 1.0) * radius;
    const auto base = dvec2(1.0, 0.0) * (radius - 1.0);

    const auto rotationMatrix = dmat2(cos(phi), -sin(phi), sin(phi), cos(phi));
    const auto angle = base * rotationMatrix;
    const auto direction = glm::normalize(-angle);
    spdlog::debug(
        "Center: ({:.2f}, {:.2f}), Base: ({:.2f}, {:.2f}), Angle: ({:.4f}, {:.4f}), Direction: "
        "({:.4f}, {:.4f})",
        center.x,
        center.y,
        base.x,
        base.y,
        angle.x,
        angle.y,
        direction.x,
        direction.y
    );

    const auto tangentCenter = center + angle;
    const auto tangentDirection = normalize(dvec2(-angle.y, angle.x));
    spdlog::debug(
        "Tangent Center: ({:.2f}, {:.2f}), Tangent Direction: ({:.4f}, {:.4f})",
        tangentCenter.x,
        tangentCenter.y,
        tangentDirection.x,
        tangentDirection.y
    );

    const auto stepSize = static_cast<double>(imageSize) / numRays;
    auto rays = vector<Ray>();
    rays.reserve(numRays);

    for (size_t i = 0; i < numRays; ++i) {
        const auto origin = tangentCenter + tangentDirection * (i * stepSize - radius);
        rays.push_back(Ray(origin, direction, imageSize));
    }

    return rays;
}

double RayTracer::traceRay(const Ray& ray) const {
    const auto origin = ray.getOrigin();
    const auto direction = ray.getDirection();
    const auto length = ray.getLength();

    spdlog::trace(
        "Tracing Ray( origin = ({:.2f}, {:.2f}), direction = ({:.4f}, {:.4f}), "
        "length = {} )",
        origin.x,
        origin.y,
        direction.x,
        direction.y,
        length
    );

    const auto imageSize = m_densityMap.getSize();
    const auto scanField = cv::Rect2d(0, 0, imageSize, imageSize);

    auto tEntry = -numeric_limits<double>::infinity();
    auto tExit = numeric_limits<double>::infinity();

    if (direction.x != 0.0) {
        const auto tAtXMin = (scanField.x - origin.x) / direction.x;
        const auto tAtXMax = (scanField.width - origin.x) / direction.x;
        const auto tEntryX = min(tAtXMin, tAtXMax);
        const auto tExitX = max(tAtXMin, tAtXMax);

        tEntry = max(tEntry, tEntryX);
        tExit = min(tExit, tExitX);
    }
    else if (origin.x < scanField.x || origin.x > scanField.width) {
        spdlog::trace("Ray is parallel to x-axis and outside image bounds. Returning 0.0");
        return 0.0;
    }

    if (direction.y != 0.0) {
        const auto tAtYMin = (scanField.y - origin.y) / direction.y;
        const auto tAtYMax = (scanField.height - origin.y) / direction.y;
        const auto tEntryY = min(tAtYMin, tAtYMax);
        const auto tExitY = max(tAtYMin, tAtYMax);

        tEntry = max(tEntry, tEntryY);
        tExit = min(tExit, tExitY);
    }
    else if (origin.y < scanField.y || origin.y > scanField.height) {
        spdlog::trace("Ray is parallel to y-axis and outside image bounds. Returning 0.0");
        return 0.0;
    }

    if (tExit < tEntry || tExit < 0.0) {
        spdlog::trace("No valid intersection with image boundaries. Returning 0.0");
        return 0.0;
    }

    const auto tStart = max(tEntry, 0.0);
    const auto tEnd = tExit;

    spdlog::trace(
        "Integrating from tStart={:.4f} to tEnd={:.4f} across image boundaries.", tStart, tEnd
    );

    const auto delta_t = 0.5;
    auto step = 0;
    spdlog::trace("using delta_t={:.4f} for integration.", delta_t);

    double totalDensity = 0.0;

    for (auto t = tStart; t < tEnd; t += delta_t, step++) {
        const auto p = origin + t * direction;

        const auto x = static_cast<size_t>(std::floor(p.x));
        const auto y = static_cast<size_t>(std::floor(p.y));

        // if (x >= 0 && x < imageSize && y >= 0 && y < imageSize) {
        if (x < imageSize && y < imageSize) {
            const auto density = m_densityMap.getDensity(x, y);
            totalDensity += density * delta_t;
            spdlog::trace(
                "Accumulated density: {:.4f} * {:.4f} = {:.4f}, Total Density: {:.4f}",
                density,
                delta_t,
                density * delta_t,
                totalDensity
            );
        }

        else {
            spdlog::trace("Point ({}, {}) is out of bounds. Skipping.", x, y);
        }
    }

    spdlog::trace("Final Total Density: {:.4f}", totalDensity);
    return totalDensity;
}
