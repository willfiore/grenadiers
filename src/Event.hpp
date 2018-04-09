#pragma once

#include <boost/any.hpp>
#include <vector>

#include <glm/vec2.hpp>

class Player;
class Grenade;
class Powerup;

struct Event
{
  enum Type {
    KEY_PRESS,
    GAME_START,
    PLAYER_DEATH,
    PLAYER_THROW_GRENADE,
    PLAYER_DETONATE_GRENADE,
    POWERUP_LAND,
    POWERUP_PICKUP,
    EXPLOSION
  };

  Event(Type t);

  Type type;
  double timestamp;
  boost::any data;
};

// EXPLOSION
struct EvdGrenadeExplosion {
  const Grenade* grenade;
};

// POWERUP_LAND
struct EvdPowerupLand {
  const Powerup* powerup;
};

// POWERUP_PICKUP
struct EvdPowerupPickup {
  int powerupType;
  int playerID;
};

// PLAYER_DEATH
struct EvdPlayerDeath {
  const Player* player;
};

// PLAYER_THROW_GRENADE
struct EvdPlayerThrowGrenade {
  const Player* player;
};

// PLAYER_DETONATE_GRENADE
struct EvdPlayerDetonateGrenade {
  const Player* player;
};
