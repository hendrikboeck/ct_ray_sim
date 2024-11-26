#pragma once
/**
 * @file RayTracer.hpp
 * @brief This file defines the RayTracer class.
 */

#include <glm/glm.hpp>
#include <vector>

#include "DensityMap.hpp"
#include "Ray.hpp"

/**
 * @class RayTracer
 * @brief Traces rays through a density map and calculates the total density.
 */
class RayTracer {
  public:
    /**
     * @brief Constructs a RayTracer object with the provided density map.
     *
     * @param densityMap The density map to use for ray tracing.
     */
    RayTracer(const DensityMap& densityMap);

    /**
     * @brief Constructs a RayTracer object with moving the provided density map.
     *
     * This constructor is useful when you want to transfer ownership of the density map without
     * copying, which can improve performance.
     *
     * @param densityMap The density map to move.
     */
    RayTracer(DensityMap&& densityMap);

    // Defaulted copy constructor and copy assignment operator
    RayTracer(const RayTracer&) = default;
    RayTracer& operator=(const RayTracer&) = default;

    // Defaulted move constructor and move assignment operator
    RayTracer(RayTracer&&) noexcept = default;
    RayTracer& operator=(RayTracer&&) noexcept = default;

    /**
     * @brief Sets up rays for the specified angle. The number of rays is specified by the numRays
     * parameter. The rays will be distributed equaly along the tangent at the angle phi. The
     * tangent will be of length equal to the size of the density map. The tangent will be centered
     * at the angle phi. Rays originate from the center of a simulated detector.
     *
     * @param phi The angle in radians.
     * @param numRays The number of rays to set up.
     * @return The vector of calculated rays. (directions are normalized)
     */
    std::vector<Ray> setupRays(const double phi, const std::size_t numRays) const;

    /**
     * @brief Traces the specified ray through the density map and returns the total density.
     *
     * @param ray The ray to trace.
     * @return The total density along the ray.
     */
    double traceRay(const Ray& ray) const;

  private:
    const DensityMap& m_densityMap;
};
