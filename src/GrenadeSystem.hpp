#pragma once

#include <vector>

#include "Grenade.hpp"
#include "Event.hpp"

class Terrain;

class GrenadeSystem {
public:
  GrenadeSystem(const Terrain*);

  void update(double dt);
  const std::vector<Grenade>& getGrenades() const { return grenades; }

private:

  void onPlayerFireWeapon(Event);
  void onPlayerSecondaryFireWeapon(Event);

  void grenadeHitGround(Grenade&, glm::vec2);
  void explodeGrenade(Grenade&);

  Grenade& spawnGrenade(Grenade::Type t = Grenade::Type::STANDARD);

  std::vector<Grenade> grenades;
  std::vector<Grenade> grenadesToSpawn;
  const Terrain* terrain;
};
