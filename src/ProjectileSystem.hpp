#pragma once

#include <vector>

#include "Projectiles.hpp"
#include "Event.hpp"

class Terrain;

class ProjectileSystem {
public:
  ProjectileSystem(const Terrain*);

  void update(float dt);

  void updateGrenades(float dt);
  void updateMissiles(float dt);

  const std::vector<Grenade>& getGrenades() const { return grenades; }
  const std::vector<Missile>& getMissiles() const { return missiles; }

  void spawnGrenade(glm::vec2 p, glm::vec2 v);
  void spawnMissile(glm::vec2 p, glm::vec2 v);

private:

  void onPlayerFireWeapon(Event);

  std::vector<Grenade> grenades;
  std::vector<Missile> missiles;

  const Terrain* terrain;
};
