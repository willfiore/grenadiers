#pragma once

#include <glm/vec2.hpp>

struct Grenade
{
  static constexpr float SIZE = 3.f;
  static constexpr float LIFETIME = 1.8f;
  static constexpr float EXPLOSION_RADIUS = 140.f;

  glm::vec2 position;
  glm::vec2 velocity;
  float age;
};
