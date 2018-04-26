#pragma once

#include <vector>

#include "Grenade.hpp"

struct Event;
class Terrain;
class TimescaleSystem;
class PlayerSystem;

class GrenadeSystem {
public:
  GrenadeSystem(
      const Terrain&,
      const TimescaleSystem&,
      const PlayerSystem&
      );

  void update(double dt);
  const std::vector<Grenade>& getGrenades() const { return grenades; }

private:

  void onPlayerThrowGrenade(const Event&);
  void onPlayerDetonateGrenade(const Event&);

  void grenadeHitGround(Grenade&, glm::vec2);
  void explodeGrenade(Grenade&);
  void fizzleGrenade(Grenade&);

  Grenade& spawnGrenade(Grenade::Type t = Grenade::Type::STANDARD);

  std::vector<Grenade> grenades;
  std::vector<Grenade> grenadesToSpawn;

  const Terrain& terrain;
  const TimescaleSystem& timescaleSystem;
  const PlayerSystem& playerSystem;
};
