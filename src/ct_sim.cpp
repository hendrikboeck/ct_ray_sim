#include "ct_sim.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <cmath>
#include <filesystem>
#include <iomanip>
#include <iostream>

using glm::dmat2;
using glm::dvec2;
using std::size_t;
namespace fs = std::filesystem;

Simulation::Simulation(const std::string& imagePath, size_t angles) : m_numAngles{ angles } {
  spdlog::info("Initializing CtSim with imagePath: {} and angles: {}", imagePath, m_numAngles);
  loadDensityMap(imagePath);
  m_radius = static_cast<double>(m_imageSize) / 2.0;
  spdlog::info("Image Size: {}, Radius: {}", m_imageSize, m_radius);

  m_projections = cv::Mat(
    static_cast<int32_t>(m_imageSize), static_cast<int32_t>(m_numAngles), CV_64F, cv::Scalar(0)
  );
  spdlog::info(
    "Initialized projections matrix with size: {}x{}", m_projections.rows, m_projections.cols
  );
}

auto Simulation::loadDensityMap(const std::string& imagePath) -> void {
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

  m_imageSize = static_cast<size_t>(m_densityMap.rows);
  spdlog::info("Image size set to: {}x{}", m_imageSize, m_imageSize);

  if (spdlog::get_level() <= spdlog::level::debug) {
    spdlog::debug("Sample density values (limited to 10x10, centered):");
    const auto center = m_imageSize / 2.0;
    for (size_t y = center - 5; y < center + 5; y++) {
      std::string row;
      for (size_t x = center - 5; x < center + 5; x++)
        row += fmt::format("{:.2f} ", m_densityMap.at<double>(y, x));
      spdlog::debug("{}", row);
    }

    cv::Mat density_display;
    m_densityMap.convertTo(density_display, CV_8U, 255.0);
    if (cv::imwrite("debug_density_map.png", density_display))
      spdlog::info("Saved debug density map as 'debug_density_map.png'.");
    else
      spdlog::error("Failed to save debug density map as 'debug_density_map.png'.");
  }
}

auto Simulation::getDensity(size_t x, size_t y) const -> double {
  if (x >= m_imageSize || y >= m_imageSize) {
    spdlog::warn("Access out of bounds at ({}, {}), returning 0.0", x, y);
    return 0.0;
  }
  double density = m_densityMap.at<double>(y, x);
  spdlog::trace("Density at ({}, {}): {:.4f}", x, y, density);
  return density;
}

auto Simulation::run() -> void {
  spdlog::info("Starting CT simulation with {} angles.", m_numAngles);
  const auto center = dvec2{ 1.0, 1.0 } * m_radius;
  const auto baseVec = dvec2{ 1.0, 0.0 } * (m_radius - 1.0);
  spdlog::info("Center: ({:.2f}, {:.2f})", center.x, center.y);
  spdlog::info("Base Vector: ({:.2f}, {:.2f})", baseVec.x, baseVec.y);

  for (size_t i = 0; i < m_numAngles; i++) {
    double angle_deg = static_cast<double>(i) * (360.0 / m_numAngles);
    const auto angle = glm::radians(angle_deg);
    const auto rot = dmat2(glm::cos(angle), -glm::sin(angle), glm::sin(angle), glm::cos(angle));

    const auto rv = baseVec * rot;
    spdlog::debug("Angle {}: {:.2f} degrees ({:.4f} radians)", i, angle_deg, angle);
    spdlog::debug("Rotated Vector: ({:.4f}, {:.4f})", rv.x, rv.y);

    simulateRayColumn(center, rv, i);
  }
  spdlog::info("CT simulation completed.");
}

auto Simulation::simulateRayColumn(const dvec2& c, const dvec2& a, const size_t col) -> void {
  spdlog::debug("Simulating ray column {}", col);
  const auto tc = c + a;
  const auto td = glm::normalize(dvec2{ -a.y, a.x });
  const auto an = glm::normalize(-a);
  spdlog::debug(
    "tc: ({:.2f}, {:.2f}), td: ({:.4f}, {:.4f}), an: ({:.4f}, {:.4f})",
    tc.x,
    tc.y,
    td.x,
    td.y,
    an.x,
    an.y
  );

  for (size_t i = 0; i < m_imageSize; i++) {
    double projection =
      traceRay(tc + td * (static_cast<double>(i) - static_cast<double>(m_imageSize) / 2.0), an);
    if (i < static_cast<size_t>(m_projections.rows)
        && col < static_cast<size_t>(m_projections.cols))
    {
      m_projections.at<double>(i, col) = projection;
      spdlog::debug("Projection[{}, {}] = {:.4f}", i, col, projection);
    }
    else {
      spdlog::error("Projection index out of bounds for [{}, {}]", i, col);
    }
  }
}

auto Simulation::traceRay(const glm::dvec2& startPoint, const glm::dvec2& direction) const
  -> double {
  spdlog::trace(
    "Tracing ray from ({:.2f}, {:.2f}) in direction ({:.2f}, {:.2f})",
    startPoint.x,
    startPoint.y,
    direction.x,
    direction.y
  );

  double tmin = -std::numeric_limits<double>::infinity();
  double tmax = std::numeric_limits<double>::infinity();

  const double xmin = 0.0;
  const double xmax = static_cast<double>(m_imageSize);
  const double ymin = 0.0;
  const double ymax = static_cast<double>(m_imageSize);

  if (direction.x != 0.0) {
    double tx1 = (xmin - startPoint.x) / direction.x;
    double tx2 = (xmax - startPoint.x) / direction.x;
    double tmin_x = std::min(tx1, tx2);
    double tmax_x = std::max(tx1, tx2);
    tmin = std::max(tmin, tmin_x);
    tmax = std::min(tmax, tmax_x);
  }
  else if (startPoint.x < xmin || startPoint.x > xmax) {
    spdlog::trace("Ray is parallel to x-axis and outside image bounds. Returning 0.0");
    return 0.0;
  }

  if (direction.y != 0.0) {
    double ty1 = (ymin - startPoint.y) / direction.y;
    double ty2 = (ymax - startPoint.y) / direction.y;
    double tmin_y = std::min(ty1, ty2);
    double tmax_y = std::max(ty1, ty2);
    tmin = std::max(tmin, tmin_y);
    tmax = std::min(tmax, tmax_y);
  }
  else if (startPoint.y < ymin || startPoint.y > ymax) {
    spdlog::trace("Ray is parallel to y-axis and outside image bounds. Returning 0.0");
    return 0.0;
  }

  if (tmax < tmin || tmax < 0.0) {
    spdlog::trace("No valid intersection with image boundaries. Returning 0.0");
    return 0.0;
  }

  double t_start = std::max(tmin, 0.0);
  double t_end = tmax;

  spdlog::trace(
    "Integrating from t_start={:.4f} to t_end={:.4f} across image boundaries.", t_start, t_end
  );

  const double delta_t = 0.5;
  spdlog::trace("Using delta_t={:.4f} for integration.", delta_t);

  double total_density = 0.0;

  double t = t_start;
  size_t step = 0;
  while (t < t_end) {
    glm::dvec2 p = startPoint + t * direction;

    int x = static_cast<int>(std::floor(p.x));
    int y = static_cast<int>(std::floor(p.y));

    if (x >= 0 && x < static_cast<int>(m_imageSize) && y >= 0 && y < static_cast<int>(m_imageSize))
    {
      double density = getDensity(static_cast<size_t>(x), static_cast<size_t>(y));
      total_density += density * delta_t;
      spdlog::trace(
        "Accumulated density: {:.4f} * {:.4f} = {:.4f}, Total Density: {:.4f}",
        density,
        delta_t,
        density * delta_t,
        total_density
      );
    }
    else {
      spdlog::trace("Point ({}, {}) is out of bounds. Skipping.", x, y);
    }

    t += delta_t;
    step++;
  }

  if (t < t_end + 1e-6) {
    double remaining = t_end - (t_start + (step * delta_t));
    if (remaining > 0.0) {
      glm::dvec2 p = startPoint + t * direction;
      int x = static_cast<int>(std::floor(p.x));
      int y = static_cast<int>(std::floor(p.y));

      if (x >= 0 && x < static_cast<int>(m_imageSize) && y >= 0
          && y < static_cast<int>(m_imageSize))
      {
        double density = getDensity(static_cast<size_t>(x), static_cast<size_t>(y));
        total_density += density * remaining;
        spdlog::trace(
          "Accumulated density (partial step): {:.4f} * {:.4f} = {:.4f}, Total Density: {:.4f}",
          density,
          remaining,
          density * remaining,
          total_density
        );
      }
      else {
        spdlog::trace("Partial step point ({}, {}) is out of bounds. Skipping.", x, y);
      }
    }
  }

  spdlog::trace("Final Total Density: {:.4f}", total_density);
  return total_density;
}

auto Simulation::saveProjectionImage(const std::string& outputDir) const -> void {
  spdlog::info("Starting reconstruction of the image from projections.");

  cv::Mat reconstructedImage(
    static_cast<int>(m_imageSize), static_cast<int>(m_imageSize), CV_64F, cv::Scalar(0)
  );

  const double center = static_cast<double>(m_imageSize) / 2.0;

  for (size_t i = 0; i < m_numAngles; ++i) {
    double angle_deg = static_cast<double>(i) * (360.0 / m_numAngles);
    double angle_rad = glm::radians(angle_deg);
    double cos_angle = std::cos(angle_rad);
    double sin_angle = std::sin(angle_rad);

    spdlog::debug("Processing angle {} ({} degrees)", i, angle_deg);

    for (int y = 0; y < static_cast<int>(m_imageSize); ++y) {
      for (int x = 0; x < static_cast<int>(m_imageSize); ++x) {
        double x_rel = static_cast<double>(x) - center;
        double y_rel = static_cast<double>(y) - center;

        double t = -x_rel * sin_angle + y_rel * cos_angle;

        double detector_center = static_cast<double>(m_imageSize) / 2.0;
        double detector_index = t + detector_center;

        int index0 = static_cast<int>(std::floor(detector_index));
        int index1 = index0 + 1;
        double weight1 = detector_index - static_cast<double>(index0);
        double weight0 = 1.0 - weight1;

        double projectionValue = 0.0;

        if (index0 >= 0 && index1 < static_cast<int>(m_imageSize)) {
          double value0 = m_projections.at<double>(index0, static_cast<int>(i));
          double value1 = m_projections.at<double>(index1, static_cast<int>(i));
          projectionValue = weight0 * value0 + weight1 * value1;
        }
        else if (index0 >= 0 && index0 < static_cast<int>(m_imageSize)) {
          projectionValue = m_projections.at<double>(index0, static_cast<int>(i));
        }
        else if (index1 >= 0 && index1 < static_cast<int>(m_imageSize)) {
          projectionValue = m_projections.at<double>(index1, static_cast<int>(i));
        }
        else {
          continue;
        }

        reconstructedImage.at<double>(y, x) += projectionValue;
      }
    }
  }

  cv::normalize(reconstructedImage, reconstructedImage, 0.0, 1.0, cv::NORM_MINMAX);
  spdlog::info("Reconstructed image normalized.");

  cv::Mat reconstructedImage8U;
  reconstructedImage.convertTo(reconstructedImage8U, CV_8U, 255.0);
  spdlog::info("Reconstructed image converted to 8-bit format.");

  cv::Mat reconstructedImage16U;
  reconstructedImage.convertTo(reconstructedImage16U, CV_16U, 65535.0);
  spdlog::info("Reconstructed image converted to 16-bit format.");

  if (!fs::exists(outputDir)) {
    if (!fs::create_directory(outputDir)) {
      spdlog::error("Failed to create output directory: {}", outputDir);
      throw std::runtime_error("Failed to create output directory.");
    }
    spdlog::info("Created output directory: {}", outputDir);
  }

  const auto simPath = std::string(fs::path(outputDir) / "simulation.png");
  if (cv::imwrite(simPath, reconstructedImage8U))
    spdlog::info("Reconstructed image saved to '{}'.", simPath);
  else
    spdlog::error("Failed to save reconstructed image to '{}'.", simPath);

  const auto sim16bitPath = std::string(fs::path(outputDir) / "simulation_16bit.png");
  if (cv::imwrite(sim16bitPath, reconstructedImage16U))
    spdlog::info("Reconstructed 16bit image saved to '{}'.", sim16bitPath);
  else
    spdlog::error("Failed to save reconstructed 16bit image to '{}'.", sim16bitPath);

  const auto scanArrayPath = std::string(fs::path(outputDir) / "scan_array.png");
  if (cv::imwrite(scanArrayPath, m_projections))
    spdlog::info("Scan array saved to '{}'.", scanArrayPath);
  else
    spdlog::error("Failed to save scan array to '{}'.", scanArrayPath);
}
