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
      Event e(Event::EXPLOSION);
      e << g.position << Grenade::EXPLOSION_RADIUS;
      EventManager::Send(e);
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

    // Terrain explosion
    if (m.position.y < terrain->getHeight(m.position.x)) {
      Event e(Event::EXPLOSION);
      e << m.position << Missile::EXPLOSION_RADIUS;
      EventManager::Send(e);
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
  Player p = boost::any_cast<Player>(e[0]);
  Weapon w = boost::any_cast<Weapon>(e[1]);

  if (w == Weapon::GRENADE) {
    float strength = 500.f;

    // Inherit player velocity
    glm::vec2 velocity = p.velocity * 0.66f;
    velocity.x += strength * glm::cos(p.aimDirection);
    velocity.y += strength * -glm::sin(p.aimDirection);

    spawnGrenade(p.position, velocity);
  }

  if (w == Weapon::MISSILE) {
    float strength = 1500.f;

    glm::vec2 velocity;
    velocity.x += strength * glm::cos(p.aimDirection);
    velocity.y += strength * -glm::sin(p.aimDirection);

    spawnMissile(p.position, velocity);
  }
}
