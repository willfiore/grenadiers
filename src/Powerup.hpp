#pragma once

#include <glm/vec2.hpp>

struct Powerup
{
  int type;
  glm::vec2 position;
  glm::vec2 targetPosition;
  float angle;

  bool landed;
  
  bool dirty_awaitingRemoval{false};
};
