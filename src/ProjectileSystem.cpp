#include "ProjectileSystem.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include "imgui.h"
#include "Console.hpp"

#include "Terrain.hpp"
#include "EventManager.hpp"
#include "Player.hpp"

ProjectileSystem::ProjectileSystem(const Terrain* t) :
  terrain(t)
{
  EventManager::Register(Event::PLAYER_FIRE_WEAPON,
      std::bind(&ProjectileSystem::onPlayerFireWeapon, this, _1));

  EventManager::Register(Event::PLAYER_SECONDARY_FIRE_WEAPON,
      std::bind(&ProjectileSystem::onPlayerSecondaryFireWeapon, this, _1));
}

void ProjectileSystem::update(float dt)
{
  for (auto i = projectiles.begin(); i != projectiles.end();) {
    Projectile& p = *i;
    p.age += dt;
    p.velocity += p.acceleration * dt;
    glm::vec2 newPosition = p.position + p.velocity * dt;

    if (!p.dirty_justBounced) {
      std::vector<LineSegment> tSegments =
	terrain->getSegmentsInRange(p.position.x, newPosition.x);

      bool foundIntersection = false;
      for (auto& s : tSegments) {

	auto intersection =
	  geo::intersect(s.first, s.second, p.position, newPosition);

	if (intersection.first) {
	  foundIntersection = true;
	  p.dirty_justBounced = true;

	  newPosition = intersection.second;
	  float terrainAngle = terrain->getAngle(p.position.x);
	  float projectileAngle = glm::atan(p.velocity.y, p.velocity.x);
	  float rotateAngle = 2 * (terrainAngle - projectileAngle);
	  p.velocity = 0.5f * glm::rotate(p.velocity, rotateAngle);
	  break;
	}
      }

      // Failsafe
      if (!foundIntersection && newPosition.y < terrain->getHeight(newPosition.x)) {
	newPosition.y = terrain->getHeight(newPosition.x);
	float terrainAngle = terrain->getAngle(p.position.x);
	float projectileAngle = glm::atan(p.velocity.y, p.velocity.x);
	float rotateAngle = 2 * (terrainAngle - projectileAngle);
	p.velocity = 0.5f * glm::rotate(p.velocity, rotateAngle);
      }
    } else {
      p.dirty_justBounced = false;
    }

    p.position = newPosition;

    if (p.age > GRENADE_LIFETIME) {
      EvdExplosion d;
      d.position = p.position;
      d.radius = GRENADE_EXPLOSION_RADIUS;
      d.damage = GRENADE_MAX_DAMAGE;
      EventManager::Send(Event::EXPLOSION, d);

      projectiles.erase(i);
      continue;
    }

    ++i;
  }
}

void ProjectileSystem::onPlayerFireWeapon(Event e)
{
  auto d = boost::any_cast<EvdPlayerFireWeapon>(e.data);

  if (d.weapon == Weapon::GRENADE) {
    Projectile& p = spawnProjectile(Projectile::GRENADE);
    p.owner = d.player.id;

    float strength = 600.f;
    p.position = d.player.getCenterPosition();
    p.velocity = 0.33f * d.player.velocity;
    p.velocity.x += strength * glm::cos(d.player.aimDirection);
    p.velocity.y += strength * -glm::sin(d.player.aimDirection);
    p.acceleration = {0.f, -1000.f};
  }

  if (d.weapon == Weapon::MISSILE) {
    Projectile& p = spawnProjectile(Projectile::MISSILE);
    p.owner = d.player.id;

    float strength = 1500.f;
    p.position = d.player.getCenterPosition();
    p.velocity.x += strength * glm::cos(d.player.aimDirection);
    p.velocity.y += strength * -glm::sin(d.player.aimDirection);
  }
}

void ProjectileSystem::onPlayerSecondaryFireWeapon(Event e)
{
  auto d = boost::any_cast<EvdPlayerSecondaryFireWeapon>(e.data);

  for (auto it = projectiles.begin(); it != projectiles.end();) {
    const Projectile& p = *it;

    if (p.owner == d.player.id) {
      EvdExplosion d;
      d.position = p.position;
      d.radius = 0.7f * GRENADE_EXPLOSION_RADIUS;
      d.damage = 0.5f * GRENADE_MAX_DAMAGE;
      d.knockback = 100.f;
      EventManager::Send(Event::EXPLOSION, d);

      projectiles.erase(it);
      continue;
    }
    ++it;
  }
}

Projectile& ProjectileSystem::spawnProjectile(Projectile::Type type)
{
  projectiles.emplace_back(Projectile(type));
  return projectiles.back();
}

