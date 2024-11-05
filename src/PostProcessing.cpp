#include "PostProcessing.hpp"

#include <spdlog/spdlog.h>

PostProcessing::PostProcessing(const cv::Mat& image) : m_image(image) { }

PostProcessing::PostProcessing(cv::Mat&& image) : m_image(std::move(image)) { }

PostProcessing& PostProcessing::normalize() {
    cv::normalize(m_image, m_image, 0.0, 1.0, cv::NORM_MINMAX);
    return *this;
}

PostProcessing& PostProcessing::to8U() {
    m_image.convertTo(m_image, CV_8U, 255.0);
    return *this;
}

PostProcessing& PostProcessing::to16U() {
    m_image.convertTo(m_image, CV_16U, 65535.0);
    return *this;
}

const cv::Mat& PostProcessing::getRef() const {
    return m_image;
}

cv::Mat PostProcessing::getImage() const {
    return m_image.clone();
}

void PostProcessing::saveImage(const std::string& outputPath) const {
    if (cv::imwrite(outputPath, m_image))
        spdlog::info("Saved image as '{}'.", outputPath);
    else
        spdlog::error("Failed to save image as '{}'.", outputPath);
}
