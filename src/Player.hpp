#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>

#include "Weapon.hpp"

struct Player {
  Player();

  static constexpr float MOVEMENT_DEADZONE = 0.4f;
  static constexpr float MAX_SPEED = 360.f;
  static constexpr float ACCEL_X = 10000.f;
  static constexpr float ACCEL_X_NOCONTROL = 1800.f;
  static constexpr float ACCEL_X_AIRBORNE = 2000.f;
  static constexpr float ACCEL_X_AIRBORNE_NOCONTROL = 100.f;
  static constexpr float ACCEL_Y = -4000.f;
  static constexpr float MAX_DOWNHILL_ANGLE = glm::radians(45.f);
  static constexpr float MAX_UPHILL_ANGLE = glm::radians(60.f);
  static constexpr float MIN_SIDEJUMP_ANGLE = glm::radians(20.f);
  static constexpr float JUMP_VELOCITY = 1000.f;
  static constexpr float SIZE = 10.f;
  static constexpr float STARTING_HEALTH = 300.f;
  static constexpr float AIM_SPEED = 22.f;
  static constexpr float AIM_SPEED_BEAM = 0.f;
  static constexpr double GRENADE_REFRESH_TIMER = 2.0;

  glm::vec2 getCenterPosition() const;

  void giveWeapon(const Weapon&);

  // State
  //////////////////////////////////////////////

  int id;
  int controllerID;

  // Physics
  glm::vec2 position;
  glm::vec2 velocity;
  glm::vec2 acceleration;
  float angle;
  float aimDirection;

  // State
  float health;
  std::vector<Weapon> weapons;
  std::vector<Weapon>::size_type currentWeaponIndex;

  double grenadeRefreshTimer;

  // Flags
  bool alive;
  bool airborne;
  bool jumpAvailable;
  bool outOfControl;
  bool firingBeam;
  bool lastMovingRight;

  // Dirty flags
  bool dirty_justLeftGround;
};
