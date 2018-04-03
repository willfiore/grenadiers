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
      ammo = 3;
      break;
    case Type::MISSILE:
      ammo = 5;
      break;
    case Type::BEAM:
      ammo = 100;
      break;
    default:
      ammo = 0;
      break;
  }
}
