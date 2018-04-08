#include "Grenade.hpp"
#include "Random.hpp"
#include "geo.hpp"

Grenade::Grenade() :
  Grenade(Type::STANDARD)
{}

Grenade::Grenade(Type t) :
  type(t)
{
  owner = -1;
  age = 0.f;
  localTimescale = 1.f;

  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2(0.f, -1000.f);
  
  dirty_awaitingRemoval = false;
  dirty_justBounced = false;
  dirty_justCollidedWithPlayer = -1;
  
  // Default properties
  properties.lifetime = 2.f;
  properties.knockback = 0.f;
  properties.radius = 0.f;
  properties.terrainDamageModifier = 1.f;
  properties.terrainWobbleModifier = 1.f;
  properties.damage = 0.f;
  properties.numClusterFragments = 0;
  properties.manualDetonate = true;
  properties.detonateOnPlayerHit = false;
  properties.bounceOnPlayerHit = true;
  properties.detonateOnLand = false;
  properties.slowBeforeDetonate = false;

  switch(type) {
    case Type::STANDARD:
      properties.knockback = 400.f;
      properties.radius = 120.f;
      properties.damage = 100.f;
      break;
    case Type::CLUSTER:
      properties.numClusterFragments = Random::randomInt(24, 30);
      properties.radius = 0.f;
      break;
    case Type::CLUSTER_FRAGMENT:
      properties.lifetime = geo::inf<float>();
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
      properties.radius = 144.f;
      properties.terrainDamageModifier = 0.f;
      properties.terrainWobbleModifier = 0.f;
      properties.slowBeforeDetonate = true;
      properties.manualDetonate = false;
      break;
    default: break;
  }
}