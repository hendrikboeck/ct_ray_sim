#include "SimulationResult.hpp"

SimulationResult::SimulationResult(const cv::Mat& image, const cv::Mat& projections)
    : m_image(image),
      m_projections(projections) { }

SimulationResult::SimulationResult(cv::Mat&& image, cv::Mat&& projections)
    : m_image(std::move(image)),
      m_projections(std::move(projections)) { }

const cv::Mat& SimulationResult::getImage() const noexcept {
    return m_image;
}

const cv::Mat& SimulationResult::getProjections() const noexcept {
    return m_projections;
}