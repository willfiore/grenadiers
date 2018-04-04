#include "Weapon.hpp"

Weapon::Weapon() :
  Weapon(Type::GRENADE)
{}

Weapon::Weapon(Type t)
{
  type = t;

  switch (t)
  {
    case Type::GRENADE:
      maxAmmo = 3;
      break;
    case Type::MISSILE:
      maxAmmo = 5;
      break;
    case Type::BEAM:
      maxAmmo = 100;
      break;
    default:
      maxAmmo = 0;
      break;
  }
  ammo = maxAmmo;
}
