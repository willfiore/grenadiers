#pragma once

struct Weapon
{
  enum class Type {
    GRENADE,
    MISSILE,
    BEAM,
    NUM_WEAPONS
  };

  Weapon();
  Weapon(Type);

  Type type;

  int ammo;
  int maxAmmo;
};
