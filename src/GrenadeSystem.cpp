#include "GrenadeSystem.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui.h"
#include "Console.hpp"

#include "EventManager.hpp"
#include "Terrain.hpp"
#include "TimescaleSystem.hpp"
#include "PlayerSystem.hpp"
#include "Random.hpp"

GrenadeSystem::GrenadeSystem(
    const Terrain& t,
    const TimescaleSystem& ts,
    const PlayerSystem& p) :
  terrain(t),
  timescaleSystem(ts),
  playerSystem(p)
{
  EventManager::Register(Event::PLAYER_THROW_GRENADE,
      std::bind(&GrenadeSystem::onPlayerThrowGrenade, this, _1));

  EventManager::Register(Event::PLAYER_DETONATE_GRENADE,
      std::bind(&GrenadeSystem::onPlayerDetonateGrenade, this, _1));
}

void GrenadeSystem::update(double gdt)
{
  // Remove dead grenades
  grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
	[](const Grenade& g)->bool {
	return g.dirty_awaitingRemoval;
	}), grenades.end());
  
  // Add new grenades
  grenades.insert(grenades.end(),
      grenadesToSpawn.begin(), grenadesToSpawn.end());
  grenadesToSpawn.clear();

  for (auto& g : grenades) {
    double newTimescale = timescaleSystem.getTimescaleAtPosition(g.position);
    double inertia = newTimescale < g.localTimescale ? 0.3 : 0.03;
    g.localTimescale += inertia * 
      (timescaleSystem.getTimescaleAtPosition(g.position) - g.localTimescale);
    double dt = g.localTimescale * gdt;

    g.age += dt;

    // ------------
    // Physics
    // ------------

    if (g.properties.slowBeforeDetonate &&
	g.properties.lifetime - g.age < 0.5) {
      double slowFactor = glm::pow(2*(g.properties.lifetime - g.age), 2);
      if (slowFactor < 0.05) slowFactor = 0.05;
      dt *= slowFactor;
    }

    g.velocity += g.acceleration * (float)dt;

    glm::vec2 newPosition = g.position + g.velocity * (float)dt;

    // Bounce on terrain
    // ------------------
    if (g.dirty_justBounced) {
      g.dirty_justBounced = false;
    }
    else {
      std::vector<LineSegment> tSegments =
	terrain.getSegmentsInRange(g.position.x, newPosition.x);

      bool foundIntersection = false;
      for (auto& s : tSegments) {

	auto intersection =
	  geo::intersect(s.first, s.second, g.position, newPosition);

	if (intersection.first) {
	  foundIntersection = true;
	  g.dirty_justBounced = true;
	  g.dirty_justCollidedWithPlayer = -1;

	  newPosition = intersection.second;
	  grenadeHitGround(g, newPosition);
	  break;
	}
      }
      // Failsafe
      if (!foundIntersection &&
	  newPosition.y < terrain.getHeight(newPosition.x)) {
	newPosition.y = terrain.getHeight(newPosition.x);
	grenadeHitGround(g, newPosition);
      }
    }

    // Collide with players
    // --------------------
    for (const auto& p : playerSystem.getPlayers()) {
      // Don't collide inside another player
      if (g.dirty_justCollidedWithPlayer == p.id) {
	if (p.collidesWith(g.position, newPosition)) continue;
	else g.dirty_justCollidedWithPlayer = -1;
      }

      if (p.collidesWith(g.position, newPosition)) {
	if (g.properties.detonateOnPlayerHit) {
	  explodeGrenade(g);
	  break;
	}
	if (g.properties.bounceOnPlayerHit) {
	  glm::vec2 bounceDirection = glm::normalize(g.position - p.position);
	  g.velocity =
	    0.6f * glm::length(g.velocity) * bounceDirection +
	    0.2f * p.velocity;
	  g.dirty_justCollidedWithPlayer = p.id;
	  break;
	}
      }
    }

    g.position = newPosition;

    // -------------
    // Timer explode
    // -------------
    if (g.age >= g.properties.lifetime) {
      explodeGrenade(g);
    }
  }
}

void GrenadeSystem::grenadeHitGround(Grenade& g, glm::vec2 pos)
{
  if (g.properties.detonateOnLand) {
    explodeGrenade(g);
    return;
  }

  float terrainAngle = terrain.getAngle(g.position.x);
  float grenadeAngle = glm::atan(g.velocity.y, g.velocity.x);
  float rotateAngle = 2 * (terrainAngle - grenadeAngle);
  g.velocity = 0.5f * glm::rotate(g.velocity, rotateAngle);
  // g.position = pos;
}

void GrenadeSystem::explodeGrenade(Grenade& g)
{
  g.dirty_awaitingRemoval = true;

  EvdGrenadeExplosion d;
  d.grenade = &g;
  EventManager::Send(Event::EXPLOSION, d);

  for (int i = 0; i < g.properties.numClusterFragments; ++i) {
    Grenade& f = spawnGrenade(Grenade::Type::CLUSTER_FRAGMENT);
    f.owner = g.owner;
    f.position = g.position;
    f.velocity.x = 0.3f*g.velocity.x + 230.f*Random::randomFloat(-1.f, 1.f);
    f.velocity.y = 400.f*Random::randomFloat(0.5f, 1.f);
  }
}

void GrenadeSystem::onPlayerThrowGrenade(const Event& e)
{
  auto d = boost::any_cast<EvdPlayerThrowGrenade>(e.data);
  const Player* p = d.player;

  if (p->inventory.size() == 0) return;

  Grenade::Type primaryType = p->inventory[p->primaryGrenadeSlot].type;

  Grenade::Type type;
  if (!p->combinationEnabled) {
    type = primaryType;
  } else {
    Grenade::Type secondaryType = p->inventory[p->secondaryGrenadeSlot].type;
    type = Grenade::Type(Grenade::Type::_ +
	geo::uniquePair(primaryType, secondaryType));
  }

  Grenade& g = spawnGrenade(type);
  g.owner = p->id;
  g.dirty_justCollidedWithPlayer = g.owner;

  float strength = 600.f;
  g.position = p->getCenterPosition();
  g.velocity = 0.33f * p->velocity;
  g.velocity.x += strength * glm::cos(p->aimDirection);
  g.velocity.y += strength * -glm::sin(p->aimDirection);
}

void GrenadeSystem::onPlayerDetonateGrenade(const Event& e)
{
  const auto* p = boost::any_cast<EvdPlayerDetonateGrenade>(e.data).player;

  for (auto& g : grenades) {
    // Grenade secondary fire explodes grenades
    if (g.owner == p->id &&
	g.properties.manualDetonate) {
      explodeGrenade(g);
      continue;
    }
  }
}

Grenade& GrenadeSystem::spawnGrenade(Grenade::Type type)
{
  grenadesToSpawn.emplace_back(Grenade(type));
  return grenadesToSpawn.back();
}
