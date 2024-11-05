#pragma once
/**
 * @file Simulation.hpp
 * @brief This file contains the declaration of the Simulation class.
 */

#include <memory>
#include <opencv2/opencv.hpp>
#include <string>

#include "DensityMap.hpp"
#include "RayTracer.hpp"
#include "SimulationResult.hpp"

/**
 * @class Simulation
 * @brief This class represents a CT simulation. It uses a density map to simulate CT objects.
 */
class Simulation {
  public:
    /**
     * @brief Constructs a Simulation object with the provided density map.
     *
     * @param densityMap The density map to use for the simulation.
     * @see DensityMap
     */
    Simulation(const DensityMap& densityMap);

    /**
     * @brief Constructs a Simulation object with moving the provided density map.
     *
     * This constructor is useful when you want to transfer ownership of the density map
     * without copying, which can improve performance.
     *
     * @param densityMap The density map to move.
     * @see DensityMap
     */
    Simulation(DensityMap&& densityMap);

    // Defaulted copy constructor and copy assignment operator
    Simulation(const Simulation&) = default;
    Simulation& operator=(const Simulation&) = default;

    // Defaulted move constructor and move assignment operator
    Simulation(Simulation&&) noexcept = default;
    Simulation& operator=(Simulation&&) noexcept = default;

    /**
     * @brief Simulates a CT scan with the specified number of angles.
     *
     * @param numAngles The number of angles to use for the simulation.
     * @return A SimulationResult object containing the reconstructed image and projections.
     * @see SimulationResult
     */
    SimulationResult simulateCT(const std::size_t numAngles) const;

    /**
     * @brief Simulates a projection for the specified angle.
     *
     * @param phi The angle in radians.
     * @return The simulated projection.
     */
    cv::Mat simulateProjectionForAngle(const double phi) const;

    /**
     * @brief This function filters each projection using the ramp filter. It handles the necessary
     * padding and uses the Discrete Fourier Transform (DFT) functions from OpenCV.
     *
     * @param projections The projections to filter.
     * @return The filtered projections.
     */
    cv::Mat& filterProjections(cv::Mat& projections) const;

    /**
     * @brief This function back-projects the (filtered) projections to reconstruct the image.
     *
     * @param projections The (filtered) projections to back-project.
     * @return The reconstructed image.
     */
    cv::Mat backProject(const cv::Mat& projections) const;

  private:
    const DensityMap& m_densityMap;
    RayTracer m_rayTracer;
};
