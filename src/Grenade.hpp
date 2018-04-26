#pragma once

#include <glm/vec2.hpp>
#include <map>
#include <string>
#include "geo.hpp"

struct Grenade
{
  // Don't forget to add typename string in .cpp file!!
  enum Type {
    // Player ownable
    STANDARD,
    CLUSTER,
    INERTIA,
    TELEPORT,
    _1,
    
    // Not player ownable
    CLUSTER_FRAGMENT,
    _2,

    // Combination grenades
    COMBI_CLUSTER_INERTIA = _2 + geo::uniquePair(CLUSTER, INERTIA),
  };

  static std::map<Type, std::string> typeStrings;
  static const char* getTypeString(Type);

  Grenade();
  Grenade(Type);
  void setProperties(Grenade::Type);

  Type type;
  int owner;
  double spawnTimestamp;
  double age;
  double localTimescale;

  glm::vec2 position;
  glm::vec2 velocity;
  glm::vec2 acceleration;

  bool dirty_justBounced;
  bool dirty_awaitingRemoval;
  int dirty_justCollidedWithPlayer;

  // Properties
  struct {

    double lifetime;
    float knockback;
    float damage;
    float radius;
    float terrainDamageModifier;
    float terrainWobbleModifier;

    bool detonateOnDeath;
    bool manualDetonate;
    bool detonateOnLand;
    bool detonateOnPlayerHit;
    bool bounceOnPlayerHit;
    bool slowBeforeDetonate;
    int numClusterFragments;
    bool spawnInertiaZone;
    bool teleportPlayerOnDetonate;
    int maxAllowedOut;

  } properties;

};

