#pragma once
/**
 * @file SimulationResult.hpp
 * @brief This file contains the declaration of the SimulationResult class.
 */

#include <memory>
#include <opencv2/opencv.hpp>

/**
 * @class SimulationResult
 * @brief This class represents the result of a CT simulation.
 */
class SimulationResult {
  public:
    /**
     * @brief Constructs a SimulationResult object by copying the provided images.
     *
     * @param image The image matrix.
     * @param projections The projections matrix.
     */
    SimulationResult(const cv::Mat& image, const cv::Mat& projections);

    /**
     * @brief Constructs a SimulationResult object by moving the provided images.
     *
     * This constructor is useful when you want to transfer ownership of the images
     * without copying, which can improve performance.
     *
     * @param image The image matrix to move.
     * @param projections The projections matrix to move.
     */
    SimulationResult(cv::Mat&& image, cv::Mat&& projections);

    // Defaulted copy constructor and copy assignment operator
    SimulationResult(const SimulationResult&) = default;
    SimulationResult& operator=(const SimulationResult&) = default;

    // Defaulted move constructor and move assignment operator
    SimulationResult(SimulationResult&&) noexcept = default;
    SimulationResult& operator=(SimulationResult&&) noexcept = default;

    /**
     * @brief Retrieves the image matrix.
     *
     * @return A constant reference to the image matrix.
     */
    const cv::Mat& getImage() const noexcept;
    /**
     * @brief Retrieves the projections matrix.
     *
     * @return A constant reference to the projections matrix.
     */
    const cv::Mat& getProjections() const noexcept;

    /**
     * @brief Retrieves the image matrix as mutable.
     *
     * @return A mutable reference to the image matrix.
     */
    cv::Mat& getMutImage() noexcept;

    /**
     * @brief Retrieves the projections matrix as mutable.
     *
     * @return A mutable reference to the projections matrix.
     */
    cv::Mat& getMutProjections() noexcept;

  private:
    cv::Mat m_image;
    cv::Mat m_projections;
};
