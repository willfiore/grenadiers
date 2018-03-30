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
  updateGrenades(dt);
  updateMissiles(dt);
}

void ProjectileSystem::updateGrenades(float dt)
{
  for (auto i = grenades.begin(); i != grenades.end();) {
    Grenade& g = *i;

    g.age += dt;

    // Gravity
    g.velocity.y -= 1000.f * dt;
    g.position += g.velocity * dt;

    // Terrain bounce
    float terrainHeight = terrain->getHeight(g.position.x);

    if (g.position.y < terrainHeight) {
      g.position.y = terrainHeight;
      float terrainAngle = terrain->getAngle(g.position.x);
      float grenadeAngle = glm::atan(g.velocity.y, g.velocity.x);

      float rotateAngle = 2 * (terrainAngle - grenadeAngle);

      g.velocity = 0.5f * glm::rotate(g.velocity, rotateAngle);

    }

    if (g.age > Grenade::LIFETIME) {
      EvdExplosion d;
      d.position = g.position;
      d.radius = Grenade::EXPLOSION_RADIUS;
      d.damage = 60.f;
      EventManager::Send(Event::EXPLOSION, d);

      grenades.erase(i);
      continue;
    }
    ++i;
  }
}

void ProjectileSystem::updateMissiles(float dt)
{
  for (auto i = missiles.begin(); i != missiles.end();) {
    Missile& m = *i;

    m.position += m.velocity * dt;

    // Explode on terrain hit
    if (m.position.y < terrain->getHeight(m.position.x)) {

      EvdExplosion d;
      d.position = m.position;
      d.radius = Missile::EXPLOSION_RADIUS;
      d.damage = 80.f;
      EventManager::Send(Event::EXPLOSION, d);

      missiles.erase(i);
      continue;
    }
    ++i;

  }
}

void ProjectileSystem::spawnGrenade(glm::vec2 p, glm::vec2 v)
{
  Grenade g;
  g.position = p;
  g.velocity = v;
  g.age = 0.f;
  grenades.push_back(g);
}

void ProjectileSystem::spawnMissile(glm::vec2 p, glm::vec2 v)
{
  Missile m;
  m.position = p;
  m.velocity = v;
  missiles.push_back(m);
}

void ProjectileSystem::onPlayerFireWeapon(Event e)
{
  auto d = boost::any_cast<EvdPlayerFireWeapon>(e.data);

  if (d.weapon == Weapon::GRENADE) {
    float strength = 500.f;

    // Inherit player velocity
    glm::vec2 velocity = d.player.velocity * 0.66f;
    velocity.x += strength * glm::cos(d.player.aimDirection);
    velocity.y += strength * -glm::sin(d.player.aimDirection);

    spawnGrenade(d.player.position, velocity);
  }

  if (d.weapon == Weapon::MISSILE) {
    float strength = 1500.f;

    glm::vec2 velocity;
    velocity.x += strength * glm::cos(d.player.aimDirection);
    velocity.y += strength * -glm::sin(d.player.aimDirection);

    spawnMissile(d.player.position, velocity);
  }
}
