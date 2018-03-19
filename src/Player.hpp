#pragma once

#include <vector>
#include <glm/vec2.hpp>

struct Player {

  static constexpr float SIZE = 10.f;
  static constexpr float SPEED = 340.f;
  static constexpr float ACCEL_FACTOR = 1.0f;
  static constexpr float ANGLE_ACCEL_FACTOR = 15.f;
  static constexpr float FALL_ACCEL = 2000.f;
  static constexpr float JUMP_SPEED = 700.f;
  static constexpr float DOUBLE_JUMP_MULTIPLIER = 0.7f;

  static constexpr float INITIAL_ATTACK_STRENGTH = 60.f;
  static constexpr float MAX_ATTACK_STRENGTH = 150.f;
  static constexpr float ATTACK_STRENGTH_GROWTH = 4.f;

  glm::vec2 position;
  glm::vec2 velocity;

  float angle;
  float goalAngle;
  float angularVelocity;

  float aimDirection;

  bool airborne;
  bool outOfControl;
  bool doubleJumpAvailable;
};
