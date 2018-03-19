#pragma once

#include <vector>

#include "Grenade.hpp"

class Terrain;

class ProjectileSystem {
public:
  ProjectileSystem(Terrain*);

  void update(float dt);
  const std::vector<Grenade>& getGrenades() const { return grenades; }

  void spawnGrenade(glm::vec2 p, glm::vec2 v);

private:
  std::vector<Grenade> grenades;
  Terrain* terrain;
};
