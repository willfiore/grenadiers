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
    z.age += dt * globalTimescale;
  }

  // Remove old zones
  zones.erase(std::remove_if(zones.begin(), zones.end(),
      [](const Zone& z) -> bool {
      return z.age > 5.0;
      }), zones.end());
}

double TimescaleSystem::getTimescaleAtPosition(glm::vec2 p) const
{
  // Pick slowest timescale
  double timescale = 1.f;
  for (const auto& z : zones) {
    if (geo::sqdist(p, z.position) < geo::sq(z.radius)) {
      timescale = glm::min(timescale, z.timescale);
    }
  }

  return timescale;
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
    z.age = 0.0;
    z.radius = g->properties.radius;
    z.timescale = 0.25f;
  }
}
