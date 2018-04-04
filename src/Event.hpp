#pragma once

#include <boost/any.hpp>
#include <vector>

#include <glm/vec2.hpp>

#include "Player.hpp"

struct Event
{
  enum Type {
    KEY_PRESS,
    GAME_START,
    PLAYER_DEATH,
    PLAYER_FIRE_WEAPON,
    PLAYER_RELEASE_WEAPON,
    PLAYER_SECONDARY_FIRE_WEAPON,
    POWERUP_PICKUP,
    EXPLOSION
  };

  Event(Type t);

  Type type;
  double timestamp;
  boost::any data;
};

struct EvdExplosion {
  glm::vec2 position;
  float radius {0.f};
  float damage {0.f};
  float knockback {0.f};
};

struct EvdPowerupPickup {
  int powerupType;
  int playerID;
};

struct EvdPlayerDeath {
  Player player;
};

struct EvdPlayerFireWeapon {
  Player player;
  Weapon weapon;
};

struct EvdPlayerSecondaryFireWeapon {
  Player player;
  Weapon weapon;
};
