#include "ProjectileSystem.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

#include <iostream>

#include "Terrain.hpp"

ProjectileSystem::ProjectileSystem(Terrain* t) :
  terrain(t)
{
}

void ProjectileSystem::update(float dt)
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
      terrain->damage(g.position, Grenade::EXPLOSION_RADIUS);
      grenades.erase(i);
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
