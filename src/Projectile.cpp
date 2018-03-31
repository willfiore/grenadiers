#include "Projectile.hpp"

Projectile::Projectile() :
  Projectile(Type::GRENADE)
{
}

Projectile::Projectile(Type t)
{
  type = t;
  owner = -1;
  age = 0.f;

  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2();
}
