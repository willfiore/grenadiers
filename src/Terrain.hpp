#pragma once

#include <vector>
#include <deque>
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

  static constexpr float PRECISION = 50.f;
  static constexpr float MAX_MODIFIERS = 4.f;

  float getMaxDepth() const { return maxDepth; }
  float getMaxWidth() const { return maxWidth; }

  float getHeight(float x) const;
  float getAngle(float x) const;

  std::vector<LineSegment> getSegmentsInRange(float x1, float  x2) const;
  std::pair<bool, glm::vec2> intersect(glm::vec2, glm::vec2) const;

  const std::vector<glm::vec2>& getPoints() const { return points; }

  void update(double t, double dt);
  void addFunc(const std::function<float(float x, double t)>&, double);

private:
  double time;

  void wobble(float x, float amplitude);
  void deform(glm::vec2 position, float radius, float depth);

  void onExplosion(const Event& e);
  void onPowerupLand(const Event& e);

  float maxDepth;
  float maxWidth;

  std::vector<glm::vec2> basePoints;
  std::deque<TerrainPointModifier> modifiers;
  std::vector<glm::vec2> points;
};
