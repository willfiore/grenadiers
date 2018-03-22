#pragma once

#include <vector>

#include "Powerup.hpp"

class Terrain;

class PowerupSystem {
public:
  PowerupSystem(const Terrain*);

  void update(float dt);
  const std::vector<Powerup>& getPowerups() const { return powerups; }

private:
  void spawnPowerup();
  std::vector<Powerup> powerups;
  const Terrain* terrain;
};
