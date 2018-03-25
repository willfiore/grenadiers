#pragma once

#include <glm/vec2.hpp>

struct Projectile
{
  glm::vec2 position;
  glm::vec2 velocity;
};

struct Grenade : public Projectile
{
  static constexpr float SIZE = 3.f;
  static constexpr float LIFETIME = 1.8f;
  static constexpr float EXPLOSION_RADIUS = 200.f;

  float age;
};

struct Missile : public Projectile
{
  static constexpr float EXPLOSION_RADIUS = 100.f;
};
