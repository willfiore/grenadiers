#pragma once

#include <glm/vec2.hpp>

struct Grenade
{
  enum Type {
    STANDARD,
    CLUSTER,
    CLUSTER_FRAGMENT,
    TYPE_COUNT
  };

  Grenade();
  Grenade(Type);

  Type type;
  int owner;
  float age;

  glm::vec2 position;
  glm::vec2 velocity;
  glm::vec2 acceleration;

  bool dirty_justBounced;
  bool dirty_awaitingRemoval;

  // Properties
  struct {

    float lifetime;
    float knockback;
    float damage;
    float radius;
    float terrainDamageModifier;
    float terrainWobbleModifier;

    bool manualDetonate;
    bool detonateOnLand;
    int numClusterFragments;

  } properties;

};
