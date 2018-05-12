#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include "Grenade.hpp"

struct GrenadeSlot {
  GrenadeSlot(Grenade::Type t, int a) :
    type(t), ammo(a)
  {}

  Grenade::Type type;
  int ammo;
};

struct Player {
  Player();

  static constexpr float MOVEMENT_DEADZONE = 0.4f;
  static constexpr float MAX_SPEED = 360.f;
  static constexpr float ACCEL_X = 10000.f;
  static constexpr float ACCEL_X_NOCONTROL = 1800.f;
  static constexpr float ACCEL_X_AIRBORNE = 2000.f;
  static constexpr float ACCEL_X_AIRBORNE_NOCONTROL = 100.f;
  static constexpr float ACCEL_Y = -3500.f;
  static constexpr float MAX_DOWNHILL_ANGLE = glm::radians(45.f);
  static constexpr float MAX_UPHILL_ANGLE = glm::radians(60.f);
  static constexpr float MIN_SIDEJUMP_ANGLE = glm::radians(20.f);
  static constexpr float JUMP_VELOCITY = 1000.f;
  static constexpr float SIZE = 10.f;
  static constexpr float STARTING_HEALTH = 100.f;
  static constexpr int STARTING_LIVES = 5;
  static constexpr float AIM_SPEED = 22.f;
  static constexpr float AIM_SPEED_BEAM = 0.f;
  static constexpr double GRENADE_REFRESH_TIMER = 2.0;
  static constexpr int INVENTORY_SIZE = 4;

  // Methods
  //////////////////////////////////////////////

  glm::vec2 getCenterPosition() const;
  // Collide with point
  bool collidesWith(glm::vec2) const;
  // Collide with line
  bool collidesWith(glm::vec2, glm::vec2) const;

  void giveGrenade(Grenade::Type, int ammo = 1);

  // State
  //////////////////////////////////////////////

  int id;
  int controllerID;

  // Physics
  double localTimescale;
  glm::vec2 position;
  glm::vec2 velocity;
  glm::vec2 acceleration;
  float angle;
  float aimDirection;

  // State
  float health;
  int lives;
  std::vector<GrenadeSlot> inventory;
  int primaryGrenadeSlot;
  int secondaryGrenadeSlot;

  bool primingGrenade;
  bool combinationEnabled;

  bool alive;
  bool respawning;
  bool ghost;
  bool undying;
  bool airborne;
  bool jumpAvailable;
  bool outOfControl;
  bool firingBeam;
  bool lastMovingRight;

  // Dirty flags
  bool dirty_justLeftGround;
};
