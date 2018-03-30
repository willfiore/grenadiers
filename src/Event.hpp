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
    PLAYER_FIRE_WEAPON,
    POWERUP_PICKUP,
    EXPLOSION
  };

  Event(Type t);

  Type type;
  float timestamp;
  boost::any data;
};

struct EvdExplosion
{
  glm::vec2 position;
  float radius;
  float damage;
};

struct EvdPowerupPickup
{
  int powerupType;
  int playerID;
};

struct EvdPlayerFireWeapon
{
  Player player;
  Weapon weapon;
};
