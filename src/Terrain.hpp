#pragma once

#include <vector>
#include <glm/vec2.hpp>

#include "Event.hpp"

class Terrain {
public:
  Terrain();

  static constexpr float PRECISION = 50.f;

  float getMaxHeight() const { return maxHeight; }
  float getMaxWidth() const { return maxWidth; }

  float getHeight(float x) const;
  float getAngle(float x) const;

  const std::vector<glm::vec2>& getPoints() const { return points; }

private:
  void onExplosion(Event e);

  float maxHeight;
  float maxWidth;

  std::vector<glm::vec2> points;
};
