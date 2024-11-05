#pragma once
/**
 * @file DensityMap.hpp
 * @brief This file contains the declaration of the DensityMap class.
 */

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @class DensityMap
 * @brief This class represents a density map for CT ray simulation.
 */
class DensityMap {
  public:
    /**
     * @brief Constructs a DensityMap object by loading the density map from the provided image
     * file.
     *
     * @param imagePath The path to the image file containing the density map.
     */
    DensityMap(const std::string& imagePath);

    /**
     * @brief Constructs a DensityMap object by loading the density map from the provided image
     * file.
     *
     * This constructor is useful when you want to transfer ownership of the density map without
     * copying, which can improve performance.
     *
     * @param imagePath The path to the image file containing the density map.
     */
    DensityMap(std::string&& imagePath);

    // Default copy constructor and copy assignment operator
    DensityMap(const DensityMap&) = default;
    DensityMap& operator=(const DensityMap&) = default;

    // Default move constructor and move assignment operator
    DensityMap(DensityMap&&) noexcept = default;
    DensityMap& operator=(DensityMap&&) noexcept = default;

    /**
     * @brief Returns the density value at the specified coordinates. If the coordinates are out of
     * bounds, a warning is logged and 0.0 is returned.
     *
     * @param x The x-coordinate.
     * @param y The y-coordinate.
     * @return The density value at the specified coordinates.
     */
    double getDensity(std::size_t x, std::size_t y) const noexcept;

    /**
     * @brief Returns the size of the density map.
     *
     * @return The size of the density map.
     */
    std::size_t getSize() const noexcept;

    /**
     * @brief Loads the density map from the provided image file.
     *
     * @param imagePath The path to the image file containing the density map.
     */
    void loadFromFilepath(const std::string& imagePath);

  private:
    cv::Mat m_densityMap;
    std::size_t m_imageSize;
};
