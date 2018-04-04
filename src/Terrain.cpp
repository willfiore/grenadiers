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

std::vector<LineSegment> Terrain::getSegmentsInRange(float x1, float x2) const
{
  std::vector<LineSegment> ret;

  bool reverse = false;
  if (x1 > x2) {
    reverse = true;
    float t = x1;
    x1 = x2;
    x2 = t;
  }

  for (size_t i = 1; i < points.size(); ++i) {
    glm::vec2 a = points[i-1];
    glm::vec2 b = points[i];

    if ((x1 > a.x && x1 < b.x) ||
	(x2 > a.x && x2 < b.x)) {
	ret.push_back({a, b});
	}
  }

  if (reverse)
    std::reverse(ret.begin(), ret.end());

  return ret;
}

void Terrain::update(double t, double dt) {
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
    const std::function<float(float, double)>& func,
    double lifetime)
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
  addFunc([=](float x, double t) -> float {

      float dt = t - e.timestamp;

      // Oscillate up and down over time
      float r = 18.f * (glm::cos(15.f*dt + glm::half_pi<float>()));

      // Fade out over time
      float mt = glm::exp(-3.5f*dt);

      // Fade out over distance
      float dx = glm::abs(x - d.position.x);
      float mx = glm::exp(-dx / 200.f);

      return r * mx * mt;
      }, 4.0);
}
