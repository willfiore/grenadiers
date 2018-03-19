#pragma once

#include <map>

#include "Shader.hpp"

class Terrain {
public:
  Terrain();

  float getHeight(float x) const;
  float getAngle(float x) const;

  float getWidth() const { return width; } 
  std::map<float, float> points;

  void damage(glm::vec2 position, float radius);

private:
  float width;
};
