#pragma once

#include <vector>

#include "Powerup.hpp"

class Terrain;
class PlayerSystem;

class PowerupSystem {
public:
  PowerupSystem(const Terrain*, const PlayerSystem*);

  void update(float dt);
  const std::vector<Powerup>& getPowerups() const { return powerups; }

private:
  void spawnPowerup();
  std::vector<Powerup> powerups;

  const Terrain* terrain;
  const PlayerSystem* playerSystem;
};
