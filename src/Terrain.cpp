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
  points = basePoints;

  EventManager::Register(Event::EXPLOSION,
      std::bind(&Terrain::onExplosion, this, _1));
}

float Terrain::getHeight(float x) const
{
  for (size_t i = 0; i < points.size(); ++i) {
    glm::vec2 p1 = points[i];

    if (x < p1.x) {

      // Before first point
      if (i == 0) return -1000.f;
      glm::vec2 p2 = points[i-1];

      float a = (x - p1.x) / (p2.x - p1.x);
      float y = p1.y + (p2.y - p1.y) * a;

      return y;
    }
  }

  return -1000.f;
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

  for (auto it = modifiers.begin(); it != modifiers.end();) {
    auto& m = *it;

    m.age += dt;
    if (m.age > m.lifetime) {
      modifiers.erase(it);
      continue;
    }

    for (auto& p : points) {
      p.y += m.func(p.x, t);
    }

    ++it;
  }
}

void Terrain::addFunc(
    const std::function<float(float, float)>& func,
    float lifetime)
{
  TerrainPointModifier m;
  m.lifetime = lifetime;
  m.age = 0.0f;
  m.func = func;

  modifiers.push_back(m);
}

void Terrain::onExplosion(Event e)
{
  auto d = boost::any_cast<EvdExplosion>(e.data);

  // Deform terrain
  for (size_t i = 0; i < basePoints.size(); ++i) {
    glm::vec2& p = basePoints[i];
    
    float distance = glm::distance(d.position, p);
    if (distance < d.radius) {
      p.y -= 0.3f * d.radius *
        glm::cos( (distance / d.radius) * glm::half_pi<float>()) *
        (1 - (p.y / maxDepth));

       if (p.y < maxDepth) p.y = maxDepth;
     }
   }

  // Wobble terrain
  addFunc([=](float x, float t) -> float {

      // Oscillate up and down over time
      float r = 14.f * -glm::cos(20.f*t);

      // Fade out over time
      float dt = t - e.timestamp;
      float mt = glm::exp(-4.f*dt);

      // Fade out over distance
      float dx = glm::abs(x - d.position.x);
      float mx = glm::exp(-dx / 200.f);

      return r * mx * mt;
      }, 4.f);
}
