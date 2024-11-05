#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "DensityMap.hpp"
#include "Ray.hpp"

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
     * @brief Sets up rays for the specified angle.
     *
     * @param phi The angle in radians.
     * @param numRays The number of rays to set up.
     * @return A vector of rays.
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
