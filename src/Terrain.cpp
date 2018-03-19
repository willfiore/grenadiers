#include "Terrain.hpp"
#include "Random.hpp"

#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "ResourceManager.hpp"

Terrain::Terrain()
{
  width = 1500.f;
  for (int i = 0; i <= width; i += Random::randomInt(80, 250)) {
    points.insert(std::make_pair(i, Random::randomInt(60, 120)));
  }

  width = points.crbegin()->first;
  points.rbegin()->second = points.begin()->second;
}

float Terrain::getHeight(float x) const
{
  for (auto i = points.begin(); i != points.end(); ++i) {
    if (x < i->first) {

      // Before first terrain point
      if (i == points.begin()) {
	return 0.f;
      }

      float x1 = std::prev(i)->first;
      float x2 = i->first;

      float y1 = std::prev(i)->second;
      float y2 = i->second;

      // Linear interp between two points
      float a = (x - x1) / (x2 - x1);
      float y = y1 + (y2-y1)*a;

      return y;
    }
  }

  // Beyond last terrain point
  return 0.f;
}

float Terrain::getAngle(float x) const {
  for (auto i = points.begin(); i != points.end(); ++i) {
    if (x < i->first) {
      if (i == points.begin()) {
	return 0.f;
      }

      float x1 = std::prev(i)->first;
      float x2 = i->first;

      float y1 = std::prev(i)->second;
      float y2 = i->second;

      float a = glm::atan( (y2 - y1) / (x2 - x1) );

      return a;
    }
  }

  return 0.f;
}

void Terrain::damage(glm::vec2 position, float radius)
{
}
