#include "geo.hpp"
#include <glm/glm.hpp>

#include <iostream>

bool geo::ccw(glm::vec2 a, glm::vec2 b, glm::vec2 c)
{
  return 
  (c.y - a.y) * (b.x - a.x) >
  (b.y - a.y) * (c.x - a.x);
}

bool geo::hasIntersection(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d)
{
  return
    (ccw(a, c, d) != ccw(b, c, d)) &&
    (ccw(a, b, c) != ccw(a, b, d));
}

std::pair<bool, glm::vec2> geo::intersect(glm::vec2 a1, glm::vec2 a2,
    glm::vec2 b1, glm::vec2 b2)
{
  std::pair<bool, glm::vec2> ret = std::make_pair(false, glm::vec2());

  glm::vec2 p = a1;
  glm::vec2 q = b1;
  glm::vec2 r = a2 - a1;
  glm::vec2 s = b2 - b1;

  float rxs = geo::cross(r, s);

  // Parallel
  if (rxs == 0.f) return ret;

  float t = geo::cross(q-p, s) / rxs;
  float u = geo::cross(q-p, r) / rxs;

  // Intersection
  if (t >= 0 && t <= 1 &&
      u >= 0 && u <= 1) {
    ret.first = true;
    ret.second = a1 + t * (a2 - a1);
  }

  return ret;
}

float geo::cross(glm::vec2 a, glm::vec2 b)
{
  glm::vec3 a3 = glm::vec3(a, 0.f);
  glm::vec3 b3 = glm::vec3(b, 0.f);
  return glm::cross(a3, b3).z;
}

