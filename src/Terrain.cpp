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
  maxDepth(-200.f),
  maxWidth(5000.f)
{
  for (float i = 0.f; i < maxWidth; i += PRECISION) {
    basePoints.push_back({i, 0.f});
  }
  maxWidth = basePoints.back().x;

  EventManager::Register(Event::EXPLOSION,
      std::bind(&Terrain::onExplosion, this, _1));
}

float Terrain::getHeight(float x) const
{
  for (size_t i = 0; i < points.size(); ++i) {
    glm::vec2 p1 = points[i];

    if (x < p1.x) {

      // Before first point
      if (i == 0) return 0.f;
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

void Terrain::update(float t, float dt) {
  points = basePoints;
  for (auto& p : points) {
    for (auto& f : funcs) {
      p.y += f(p.x, t);
    }
  }
}

void Terrain::onExplosion(Event e)
{
  glm::vec2 position = boost::any_cast<glm::vec2>(e.data[0]);
  float radius = boost::any_cast<float>(e.data[1]);

  for (size_t i = 0; i < basePoints.size(); ++i) {
    glm::vec2& p = basePoints[i];
    
    float distance = glm::distance(position, p);
    if (distance < radius) {
      p.y -= 0.3f * radius *
        glm::cos( (distance / radius) * glm::half_pi<float>()) *
        (1 - (p.y / maxDepth));

       if (p.y < maxDepth) p.y = maxDepth;
     }
   }

  // funcs.push_back([=](float x, float t) -> float {
  //     float dt = (t - e.timestamp);
  //     float dx = (x - position.x);
  //     int dir = dx > 0.f ? 1 : -1;
  //     float amp = 20.f * glm::exp(-dt) * (1.f / (1.f + glm::pow(0.01f*dx, 2)));
  //     return amp * glm::cos(-dir * 0.5f * dx + 2.f*t);
  //     });

  // funcs.push_back([=](float x, float t) -> float {
  //     float dt = (t - e.timestamp);
  //     float dx = (x - position.x);

  //     return -50.f * glm::cos(0.05f*dx);
  //     });

  // funcs.push_back([](float x, float t) -> float {
  //     return 10.f * glm::cos(0.04f * x);
  //     });
  // funcs.push_back([](float x, float t) -> float {
  //     return 10.f * glm::cos(0.06f * x);
  //     });

}
