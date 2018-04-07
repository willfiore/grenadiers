#include "TimescaleSystem.hpp"

#include "EventManager.hpp"
#include "Console.hpp"

#include <glm/glm.hpp>

#include "Grenade.hpp"

TimescaleSystem::TimescaleSystem()
{
  globalTimescale = 1.0;

  Zone& z = addZone();
  z.position = {2000.f, 200.f};
  z.radius = 144.f;
  z.timescale = 0.14f;

  EventManager::Register(Event::Type::EXPLOSION,
      std::bind(&TimescaleSystem::onExplosion, this, _1));
}

void TimescaleSystem::update(double t, double dt)
{
  // Note: t and dt realtime, not simtime, unlike
  // the update functions of other systems

  // Min timescale
  if (globalTimescale < 0.01f) globalTimescale = 0.01f;
}

double TimescaleSystem::getTimescaleAtPosition(glm::vec2 p) const
{
  double timescale = 1.f;
  for (const auto& z : zones) {
    if (glm::distance(p, z.position) < z.radius) {
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

  if (g->type == Grenade::Type::INERTIA) {
    Zone& z = addZone();
    z.position = g->position;
    z.radius = g->properties.radius;
    z.timescale = 0.17f;
  }
}
