#pragma once

#include <vector>
#include <functional>
#include <glm/vec2.hpp>
#include "geo.hpp"

#include "Event.hpp"

struct TerrainPointModifier
{
  std::function<float(float x, double t)> func;
  double age;
  double lifetime;
};

class Terrain {
public:
  Terrain();

  static constexpr float PRECISION = 45.f;

  float getMaxDepth() const { return maxDepth; }
  float getMaxWidth() const { return maxWidth; }

  float getHeight(float x) const;
  float getAngle(float x) const;

  std::vector<LineSegment> getSegmentsInRange(float x1, float  x2) const;

  const std::vector<glm::vec2>& getPoints() const { return points; }

  void update(double t, double dt);
  void addFunc(const std::function<float(float x, double t)>&, double);

private:
  void onExplosion(Event e);

  float maxDepth;
  float maxWidth;

  std::vector<glm::vec2> basePoints;
  std::vector<TerrainPointModifier> modifiers;
  std::vector<glm::vec2> points;
};
