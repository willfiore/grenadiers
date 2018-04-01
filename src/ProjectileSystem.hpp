#pragma once

#include <vector>

#include "Projectile.hpp"
#include "Event.hpp"

class Terrain;

class ProjectileSystem {
public:
  ProjectileSystem(const Terrain*);

  void update(float dt);
  const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:

  void onPlayerFireWeapon(Event);
  void onPlayerSecondaryFireWeapon(Event);

  Projectile& spawnProjectile(Projectile::Type);

  std::vector<Projectile> projectiles;
  const Terrain* terrain;
};
