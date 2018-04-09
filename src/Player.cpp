#include "Player.hpp"

#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "geo.hpp"
#include "ResourceManager.hpp"
#include "Console.hpp"

Player::Player()
{
  id = -1;
  controllerID = -1;

  // Init physics
  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2();
  angle = 0.f;
  aimDirection = 0.f;

  // Initial state
  health = STARTING_HEALTH;
  primaryGrenadeSlot = 0;

  primingGrenade = false;
  combinationEnabled = false;

  alive = true;
  airborne = false;
  jumpAvailable = true;
  outOfControl = false;
  firingBeam = false;
  lastMovingRight = true;
}

glm::vec2 Player::getCenterPosition() const
{
  glm::vec3 relativeCenter = {0.f, Player::SIZE, 0.f};
  relativeCenter = glm::rotate(relativeCenter, angle, {0.f, 0.f, 1.f});
  return position + glm::vec2(relativeCenter);
}

bool Player::collidesWith(glm::vec2 p) const
{
  glm::vec2 a = 
    glm::vec2(glm::rotate(glm::vec3(-SIZE, 0.f, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;
  glm::vec2 b =
    glm::vec2(glm::rotate(glm::vec3(SIZE, 0.f, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;
  glm::vec2 d =
    glm::vec2(glm::rotate(glm::vec3(-SIZE, 2*SIZE, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;

  float v1 = glm::dot(a-p, a-b);
  float v2 = glm::dot(a-p, a-d);
  float v3 = glm::dot(a-b, a-b);
  float v4 = glm::dot(a-d, a-d);

  if (v1 > 0 && v1 < v3 &&
      v2 > 0 && v2 < v4) {
    return true;
  }

  return false;
}

bool Player::collidesWith(glm::vec2 p1, glm::vec2 p2) const
{
  glm::vec2 a = 
    glm::vec2(glm::rotate(glm::vec3(-SIZE, 0.f, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;
  glm::vec2 b =
    glm::vec2(glm::rotate(glm::vec3(SIZE, 0.f, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;
  glm::vec2 c =
    glm::vec2(glm::rotate(glm::vec3(SIZE, 2*SIZE, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;
  glm::vec2 d =
    glm::vec2(glm::rotate(glm::vec3(-SIZE, 2*SIZE, 0.f), angle, {0.f, 0.f, 1.f}))
    + position;

  if (geo::hasIntersection(p1, p2, a, b) ||
      geo::hasIntersection(p1, p2, b, c) ||
      geo::hasIntersection(p1, p2, c, d) ||
      geo::hasIntersection(p1, p2, d, a) ||
      collidesWith(p1) || collidesWith(p2)) {
    return true;
  }
  return false;
}

void Player::giveGrenade(Grenade::Type type, int ammo)
{
  auto pre = std::find_if(inventory.begin(), inventory.end(),
      [=](const GrenadeSlot& g) -> bool {
      return g.type == type;
      });

  // Grenade doesn't already exist in inventory
  if (pre == inventory.end()) {
    if (inventory.size() < INVENTORY_SIZE) {
      inventory.emplace_back(type, ammo);
    }
  }
  else {
    pre->ammo += ammo;
  }
}

