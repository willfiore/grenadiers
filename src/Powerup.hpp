#pragma once

#include <glm/vec2.hpp>

struct Powerup
{
  enum Type {
    TEST,
    NUM_POWERUPS
  };

  Type type;
  glm::vec2 position;
  glm::vec2 targetPosition;
  float angle;

  bool landed;
  
  bool dirty_awaitingRemoval{false};
};
