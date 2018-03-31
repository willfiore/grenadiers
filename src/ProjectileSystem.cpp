#include "ProjectileSystem.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>

#include "Terrain.hpp"
#include "EventManager.hpp"
#include "Player.hpp"

ProjectileSystem::ProjectileSystem(const Terrain* t) :
  terrain(t)
{
  EventManager::Register(Event::PLAYER_FIRE_WEAPON,
      std::bind(&ProjectileSystem::onPlayerFireWeapon, this, _1));
}

void ProjectileSystem::update(float dt)
{
  for (auto i = projectiles.begin(); i != projectiles.end();) {
    Projectile& p = *i;
    p.age += dt;
    p.velocity += p.acceleration * dt;
    p.position += p.velocity * dt;

    // Check for terrain collision
    float terrainHeight = terrain->getHeight(p.position.x);
    if (p.position.y < terrainHeight) {
      p.position.y = terrainHeight;

      // Bounce
      float terrainAngle = terrain->getAngle(p.position.x);
      float projectileAngle = glm::atan(p.velocity.y, p.velocity.x);
      float rotateAngle = 2 * (terrainAngle - projectileAngle);
      p.velocity = 0.5f * glm::rotate(p.velocity, rotateAngle);
    }

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

Projectile& ProjectileSystem::spawnProjectile(Projectile::Type type)
{
  projectiles.emplace_back(Projectile(type));
  return projectiles.back();
}

