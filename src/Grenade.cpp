#include "Grenade.hpp"
#include "Random.hpp"

Grenade::Grenade() :
  Grenade(Type::STANDARD)
{}

Grenade::Grenade(Type t) :
  type(t)
{
  owner = -1;
  age = 0.f;

  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2(0.f, -1000.f);
  
  dirty_awaitingRemoval = false;
  dirty_justBounced = false;
  
  // Default properties
  properties.lifetime = 2.f;
  properties.knockback = 0.f;
  properties.radius = 0.f;
  properties.terrainDamageModifier = 1.f;
  properties.terrainWobbleModifier = 1.f;
  properties.damage = 0.f;
  properties.numClusterFragments = 0;
  properties.manualDetonate = true;
  properties.detonateOnLand = false;

  switch(type) {
    case Type::STANDARD:
      properties.knockback = 400.f;
      properties.radius = 120.f;
      properties.damage = 100.f;
      properties.lifetime = 2.f;
      break;
    case Type::CLUSTER:
      properties.numClusterFragments = Random::randomInt(16, 20);
      properties.radius = 0.f;
      break;
    case Type::CLUSTER_FRAGMENT:
      properties.lifetime = 0.f;
      properties.radius = 50.f;
      properties.damage = 30.f;
      properties.manualDetonate = false;
      properties.detonateOnLand = true;
      properties.terrainDamageModifier = 0.2f;
      properties.terrainWobbleModifier = 0.5f;
      dirty_justBounced = true;
      break;
    default: break;
  }
}
