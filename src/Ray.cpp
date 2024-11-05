#include "Ray.hpp"

Ray::Ray(const glm::dvec2& origin, const glm::dvec2& direction, std::size_t length)
    : m_origin(origin),
      m_direction(direction),
      m_length(length) { }

Ray::Ray(glm::dvec2&& origin, glm::dvec2&& direction, std::size_t length)
    : m_origin(std::move(origin)),
      m_direction(std::move(direction)),
      m_length(length) { }

const glm::dvec2& Ray::getOrigin() const noexcept {
    return m_origin;
}

const glm::dvec2& Ray::getDirection() const noexcept {
    return m_direction;
}

std::size_t Ray::getLength() const noexcept {
    return m_length;
}