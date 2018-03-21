#include "Terrain.hpp"
#include "Random.hpp"

#include <vector>
#include <map>
#include <iostream>

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include "geo.hpp"

#include "EventManager.hpp"
#include "ResourceManager.hpp"

Terrain::Terrain() :
  maxHeight(170.f),
  maxWidth(4000.f)
{
  for (float i = 0.f; i < maxWidth; i += 50.f) {
    points.push_back({i, maxHeight});
  }
  maxWidth = points.back().x;

  EventManager::Register(Event::EXPLOSION,
      std::bind(&Terrain::onExplosion, this, _1));
}

float Terrain::getHeight(float x) const
{
  for (size_t i = 0; i < points.size(); ++i) {
    glm::vec2 p1 = points[i];

    if (x < p1.x) {

      // Before first point
      if (i == 0) return p1.y;
      glm::vec2 p2 = points[i-1];

      float a = (x - p1.x) / (p2.x - p1.x);
      float y = p1.y + (p2.y - p1.y) * a;

      return y;
    }
  }

  return 0.f;
}

float Terrain::getAngle(float x) const
{
  for (size_t i = 0; i < points.size(); ++i) {
    glm::vec2 p1 = points[i];

    if (x < p1.x) {
      if (i == 0) return 0.f;
      glm::vec2 p2 = points[i-1];

      return glm::atan((p2.y - p1.y) / (p2.x - p1.x));
    }
  }

  return 0.f;
}

void Terrain::onExplosion(Event e)
{
  glm::vec2 position = boost::get<glm::vec2>(e.data[0]);
  float radius = boost::get<float>(e.data[1]);

  for (size_t i = 0; i < points.size(); ++i) {
    glm::vec2& p = points[i];
    
    float distance = glm::distance(position, p);
    if (distance < radius) {
      p.y -= 0.2f * radius *
	glm::cos( (distance / radius) * glm::half_pi<float>()) *
	(p.y / maxHeight);

      if (p.y < 0.f) p.y = 0.f;
    }
  }
}
