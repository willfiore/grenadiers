#pragma once

#include <utility>
#include <glm/vec2.hpp>
#include <limits>

typedef std::pair<glm::vec2, glm::vec2> LineSegment;

namespace geo {

  float sqdist(glm::vec2, glm::vec2);
  float sq(float);

  bool ccw(glm::vec2, glm::vec2, glm::vec2);
  bool hasIntersection(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
  std::pair<bool, glm::vec2> intersect(glm::vec2, glm::vec2, glm::vec2, glm::vec2);

  float cross(glm::vec2, glm::vec2);
  constexpr int uniquePair(int a, int b);

  template <typename T>
    T inf() {
      return std::numeric_limits<T>::infinity();
    }
}

// IMPL
constexpr int geo::uniquePair(int a, int b)
{
  // Allow 0s
  a++; b++;

  if (a < b)
    return a * b + ((b-a-1) * (b-a-1)) / 4;
  else
    return a * b + ((a-b-1) * (a-b-1)) / 4;
}
