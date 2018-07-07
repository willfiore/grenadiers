#include "TimescaleSystem.hpp"

#include "EventManager.hpp"
#include "Console.hpp"

#include <glm/glm.hpp>

#include "Grenade.hpp"

TimescaleSystem::TimescaleSystem()
{
  globalTimescale = 1.0;

  EventManager::Register(Event::Type::EXPLOSION,
      std::bind(&TimescaleSystem::onExplosion, this, _1));
}

void TimescaleSystem::update(double t, double dt)
{
  // Note: t and dt realtime, not simtime, unlike
  // the update functions of other systems

  // Min timescale
  if (globalTimescale < 0.01f) globalTimescale = 0.01f;

  for (auto& z : zones) {
    if (z.lifetime - z.age < 0.4) {
      double alpha = (z.age - z.lifetime) / 0.4 + 1;
      z.timescale = z.initialTimescale + alpha * (1.0 - z.initialTimescale);
    }

    z.age += dt * globalTimescale;
  }

  // Remove old zones
  zones.erase(std::remove_if(zones.begin(), zones.end(),
      [](const Zone& z) -> bool {
      return z.age > z.lifetime;
      }), zones.end());
}

double TimescaleSystem::getTimescaleAtPosition(glm::vec2 p) const
{
  // Pick slowest timescale
  bool insideZone = false;
  double timescale = geo::inf<double>();
  for (const auto& z : zones) {
    float sqdist = geo::sqdist(p, z.position);
    if (sqdist < geo::sq(z.radius)) {
      insideZone = true;

      // Small outer edge buffer
      double zoneTimescale = z.timescale;
      float bufferSize = 10.f;
      if (sqdist > geo::sq(z.radius-bufferSize)) {
	double alpha = (z.radius - sqrt(sqdist)) / bufferSize;
	zoneTimescale = 1.0 + alpha * (zoneTimescale - 1.0);
      }
      timescale = glm::min(timescale, zoneTimescale);
    }
  }

  return insideZone ? timescale : 1.0;
}

TimescaleSystem::Zone& TimescaleSystem::addZone()
{
  zones.emplace_back(Zone());
  return zones.back();
}

void TimescaleSystem::onExplosion(const Event& e)
{
  const Grenade* g = boost::any_cast<EvdGrenadeExplosion>(e.data).grenade;

  if (g->properties.spawnInertiaZone) {
    Zone& z = addZone();
    z.position = g->position;
    z.lifetime = 5.0;
    z.age = 0.0;
    z.radius = g->properties.radius;
    z.initialTimescale = 0.05f;
    z.timescale = z.initialTimescale;
  }
}
