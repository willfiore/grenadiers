#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "Event.hpp"

class Terrain {
public:
  Terrain();

  static constexpr float PRECISION = 50.f;

  float getMaxDepth() const { return maxDepth; }
  float getMaxWidth() const { return maxWidth; }

  float getHeight(float x) const;
  float getAngle(float x) const;

  const std::vector<glm::vec2>& getPoints() const { return points; }

  void update(float t, float dt);

private:
  void onExplosion(Event e);

  float maxDepth;
  float maxWidth;

  std::vector<glm::vec2> points;
};
