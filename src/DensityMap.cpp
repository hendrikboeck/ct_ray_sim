#include "DensityMap.hpp"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <ranges>

DensityMap::DensityMap(const std::string& imagePath) : m_densityMap(), m_imageSize(0) {
    loadFromFilepath(imagePath);
}

DensityMap::DensityMap(std::string&& imagePath) : m_densityMap(), m_imageSize(0) {
    loadFromFilepath(imagePath);
}

double DensityMap::getDensity(std::size_t x, std::size_t y) const noexcept {
    if (x >= m_imageSize || y >= m_imageSize) {
        spdlog::warn("Access out of bounds at ({}, {}), returning 0.0", x, y);
        return 0.0;
    }

    const double density = m_densityMap.at<double>(y, x);
    spdlog::trace("Density at ({}, {}): {:.4f}", x, y, density);

    return density;
}

std::size_t DensityMap::getSize() const noexcept {
    return m_imageSize;
}

void DensityMap::loadFromFilepath(const std::string& imagePath) {
    spdlog::info("Loading image from: {}", imagePath);
    m_densityMap = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

    if (m_densityMap.empty()) {
        spdlog::error("Failed to load image: {}", imagePath);
        std::exit(EXIT_FAILURE);
    }

    m_densityMap.convertTo(m_densityMap, CV_64F, 1.0 / 255.0);
    spdlog::debug("Image loaded and converted to CV_64F with scaling.");

    if (m_densityMap.rows != m_densityMap.cols) {
        spdlog::error(
            "Input image must be square (n x n). Rows: {}, Columns: {}",
            m_densityMap.rows,
            m_densityMap.cols
        );
        std::exit(EXIT_FAILURE);
    }

    m_imageSize = static_cast<std::size_t>(m_densityMap.rows);
    spdlog::info("Image size set to: {}x{}", m_imageSize, m_imageSize);

    if (spdlog::get_level() <= spdlog::level::debug) {
        spdlog::debug("Sample density values (limited to 10x10, centered):");

        const int32_t center = m_imageSize / 2.0;
        for (const int32_t y : std::views::iota(center - 5, center + 5)) {
            std::string row;
            for (const int32_t x : std::views::iota(center - 5, center + 5))
                row += fmt::format("{:.2f} ", m_densityMap.at<double>(y, x));
            spdlog::debug("{}", row);
        }
    }

    cv::Mat density_display;
    m_densityMap.convertTo(density_display, CV_8U, 255.0);

    if (cv::imwrite("debug_density_map.png", density_display))
        spdlog::info("Saved debug density map as 'debug_density_map.png'.");
    else
        spdlog::error("Failed to save debug density map as 'debug_density_map.png'.");
}
