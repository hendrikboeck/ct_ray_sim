#pragma once
/**
 * @file PostProcessing.hpp
 * @brief This file contains the declaration of the PostProcessing class.
 */

#include <opencv2/opencv.hpp>
#include <string>

/**
 * @class PostProcessing
 * @brief This class provides post-processing functionality for OpenCV images.
 */
class PostProcessing {
  public:
    /**
     * @brief Constructs a PostProcessing object with the provided image.
     *
     * @param image The image to process.
     */
    PostProcessing(const cv::Mat& image);

    /**
     * @brief Constructs a PostProcessing object with moving the provided image.
     *
     * This constructor is useful when you want to transfer ownership of the image without copying,
     * which can improve performance.
     *
     * @param image The image to move.
     */
    PostProcessing(cv::Mat&& image);

    // Default copy constructor and copy assignment operator
    PostProcessing(const PostProcessing&) = default;
    PostProcessing& operator=(const PostProcessing&) = default;

    // Default move constructor and move assignment operator
    PostProcessing(PostProcessing&&) noexcept = default;
    PostProcessing& operator=(PostProcessing&&) noexcept = default;

    /**
     * @brief Normalizes the image to the range [0, 1].
     *
     * @return A reference to the PostProcessing object.
     */
    PostProcessing& normalize();

    /**
     * @brief Converts the image to 8-bit unsigned integer.
     *
     * @return A reference to the PostProcessing object.
     */
    PostProcessing& to8U();

    /**
     * @brief Converts the image to 16-bit unsigned integer.
     *
     * @return A reference to the PostProcessing object.
     */
    PostProcessing& to16U();

    /**
     * @brief Returns the processed image.
     *
     * @return The processed image.
     */
    const cv::Mat& getRef() const;

    /**
     * @brief Returns the processed image.
     *
     * @return The processed image.
     */
    cv::Mat getImage() const;

    /**
     * @brief Saves the processed image to the specified output path.
     *
     * @param outputPath The path to save the image to.
     */
    void saveImage(const std::string& outputPath) const;

  private:
    cv::Mat m_image;
};
