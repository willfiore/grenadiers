#pragma once

#include <vector>

#include "Projectile.hpp"
#include "Event.hpp"

class Terrain;

class ProjectileSystem {
public:
  ProjectileSystem(const Terrain*);

  void update(double dt);
  const std::vector<Projectile>& getProjectiles() const { return projectiles; }

private:

  void onPlayerFireWeapon(Event);
  void onPlayerSecondaryFireWeapon(Event);

  void projectileHitGround(Projectile&, glm::vec2);
  void explodeProjectile(Projectile&);

  Projectile& spawnProjectile(Projectile::Type);

  std::vector<Projectile> projectiles;
  const Terrain* terrain;
};
