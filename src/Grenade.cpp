#include "Grenade.hpp"
#include "Random.hpp"
#include "geo.hpp"
#include "Console.hpp"

// STATICS
std::map<Grenade::Type, std::string> Grenade::typeStrings = {
  {Grenade::Type::STANDARD, "Standard"},
  {Grenade::Type::CLUSTER, "Cluster"},
  {Grenade::Type::INERTIA, "Inertia"},
  {Grenade::Type::TELEPORT, "Teleport"},
  {Grenade::Type::HOMING, "Homing"},
};

const char* Grenade::getTypeString(Type t)
{
  const auto& i = typeStrings.find(t);
  if (i == typeStrings.end()) {
    return std::to_string(t).c_str();
  }
  return i->second.c_str();
}

/////////////////////////
/////////////////////////

Grenade::Grenade() :
  Grenade(Type::STANDARD)
{}

Grenade::Grenade(Type t) :
  type(t)
{
  owner = -1;
  target = -1;

  age = 0.f;
  localTimescale = 1.f;

  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2();
  
  dirty_awaitingRemoval = false;
  dirty_justBounced = false;
  dirty_justCollidedWithPlayer = -1;

  setProperties(type);
}

void Grenade::setProperties(Grenade::Type type)
{
  // Base properties
  properties.lifetime = 2.0;

  properties.knockback = 0.f;
  properties.radius = 0.f;
  properties.terrainDamageModifier = 1.f;
  properties.terrainWobbleModifier = 1.f;
  properties.damage = 0.f;
  properties.numClusterFragments = 0;
  properties.detonateOnDeath = true;
  properties.manualDetonate = true;
  properties.detonateOnPlayerHit = false;
  properties.bounceOnPlayerHit = true;
  properties.detonateOnLand = false;
  properties.slowBeforeDetonate = false;
  properties.homing = false;
  properties.spawnInertiaZone = false;
  properties.teleportPlayerOnDetonate = false;
  properties.maxAllowedOut = geo::inf<float>();

  switch(type) {
    case Type::STANDARD:
      properties.knockback = 400.f;
      properties.radius = 160.f;
      properties.damage = 100.f;
      break;
    case Type::CLUSTER:
      properties.numClusterFragments = Random::randomInt(24, 30);
      properties.radius = 0.f;
      break;
    case Type::CLUSTER_FRAGMENT:
      properties.lifetime = geo::inf<double>();
      properties.radius = 50.f;
      properties.damage = 20.f;
      properties.manualDetonate = false;
      properties.detonateOnLand = true;
      properties.terrainDamageModifier = 0.2f;
      properties.terrainWobbleModifier = 0.5f;
      properties.detonateOnPlayerHit = true;
      dirty_justBounced = true;
      break;
    case Type::INERTIA:
      properties.radius = 200.f;
      properties.terrainDamageModifier = 0.f;
      properties.terrainWobbleModifier = 0.f;
      properties.slowBeforeDetonate = true;
      properties.spawnInertiaZone = true;
      break;
    case Type::TELEPORT:
      properties.teleportPlayerOnDetonate = true;
      properties.lifetime = geo::inf<double>();
      properties.detonateOnDeath = false;
      properties.maxAllowedOut = 1;
      break;
    case Type::HOMING:
      properties.lifetime = 5.f;
      properties.homing = true;
      properties.manualDetonate = false;
      properties.detonateOnLand = true;
      properties.detonateOnPlayerHit = true;
      properties.radius = 80.f;
      properties.damage = 10.f;
      properties.knockback = 1000.f;
      properties.detonateOnDeath = false;
      break;


    ///////////////
    // Combi
    ///////////////
    default: break;
  }
}
