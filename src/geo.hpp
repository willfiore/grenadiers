#pragma once

#include <utility>
#include <glm/vec2.hpp>
#include <limits>

typedef std::pair<glm::vec2, glm::vec2> LineSegment;

namespace geo {
// def ccw(A, B, C):
//         return (C.y - A.y) * (B.x - A.x) > (B.y - A.y) * (C.x - A.x)
//     
//     def intersect(A, B, C, D):
// return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D)

  bool ccw(glm::vec2, glm::vec2, glm::vec2);
  bool hasIntersection(glm::vec2, glm::vec2, glm::vec2, glm::vec2);
  std::pair<bool, glm::vec2> intersect(glm::vec2, glm::vec2, glm::vec2, glm::vec2);

  float cross(glm::vec2, glm::vec2);

  template <typename T>
    T inf() {
      return std::numeric_limits<T>::infinity();
    }
}
