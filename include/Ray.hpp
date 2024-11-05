#pragma once
/**
 * @file Ray.hpp
 * @brief This file defines the Ray class.
 */

#include <glm/glm.hpp>

/**
 * @class Ray
 * @brief Represents a ray with an origin, direction, and length.
 */
class Ray {
  public:
    /**
     * @brief Constructs a Ray object with the provided origin, direction, and length.
     *
     * @param origin The origin of the ray.
     * @param direction The direction of the ray.
     * @param length The length of the ray.
     */
    Ray(const glm::dvec2& origin, const glm::dvec2& direction, std::size_t length);

    /**
     * @brief Constructs a Ray object with the provided origin, direction, and length.
     *
     * This constructor is useful when you want to transfer ownership of the origin and direction
     * without copying, which can improve performance.
     *
     * @param origin The origin of the ray.
     * @param direction The direction of the ray.
     * @param length The length of the ray.
     */
    Ray(glm::dvec2&& origin, glm::dvec2&& direction, std::size_t length);

    // Defaulted copy constructor and copy assignment operator
    Ray(const Ray&) = default;
    Ray& operator=(const Ray&) = default;

    // Defaulted move constructor and move assignment operator
    Ray(Ray&&) noexcept = default;
    Ray& operator=(Ray&&) noexcept = default;

    /**
     * @brief Returns the origin of the ray.
     *
     * @return The origin of the ray.
     */
    const glm::dvec2& getOrigin() const noexcept;

    /**
     * @brief Returns the direction of the ray.
     *
     * @return The direction of the ray.
     */
    const glm::dvec2& getDirection() const noexcept;

    /**
     * @brief Returns the length of the ray.
     *
     * @return The length of the ray.
     */
    std::size_t getLength() const noexcept;

  private:
    glm::dvec2 m_origin;
    glm::dvec2 m_direction;
    std::size_t m_length;
};
