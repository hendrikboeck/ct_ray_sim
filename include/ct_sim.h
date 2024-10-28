#ifndef CTSCANSIMULATOR_H
#define CTSCANSIMULATOR_H

#include <cstdint>
#include <glm/glm.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class CtSim {
private:
  cv::Mat m_densityMap;
  cv::Mat m_projections;

  std::size_t m_imageSize;
  std::size_t m_numAngles;
  double m_radius;

  auto simulateRayColumn(const glm::dvec2& c, const glm::dvec2& a, const std::size_t col) -> void;
  auto traceRay(const glm::dvec2& startPoint, const glm::dvec2& direction) const -> double;
  auto getDensity(std::size_t x, std::size_t y) const -> double;
  auto loadDensityMap(const std::string& imagePath) -> void;

public:
  explicit CtSim(const std::string& imagePath, std::size_t angles);
  auto run() -> void;
  auto saveProjectionImage(const std::string& outputDir) const -> void;
};

#endif  // CTSCANSIMULATOR_H
