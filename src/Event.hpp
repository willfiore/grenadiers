#pragma once

#include <boost/any.hpp>
#include <vector>

#include <glm/vec2.hpp>

class Player;
class Grenade;

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

struct EvdGrenadeExplosion {
  const Grenade* grenade;
};

struct EvdPowerupPickup {
  int powerupType;
  int playerID;
};

struct EvdPlayerDeath {
  const Player* player;
};

struct EvdPlayerFireWeapon {
  const Player* player;
};

struct EvdPlayerSecondaryFireWeapon {
  const Player* player;
};
