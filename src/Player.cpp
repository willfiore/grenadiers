#include "Player.hpp"

#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "ResourceManager.hpp"

Player::Player()
{
  id = -1;
  controllerID = -1;
  currentWeaponIndex = 0;

  // Init physics
  position = glm::vec2();
  velocity = glm::vec2();
  acceleration = glm::vec2();
  angle = 0.f;
  aimDirection = 0.f;

  // Init state
  health = STARTING_HEALTH;

  alive = true;
  airborne = false;
  jumpAvailable = true;
  outOfControl = false;
  firingBeam = false;
}

glm::vec2 Player::getCenterPosition() const
{
  glm::vec3 relativeCenter = {0.f, Player::SIZE, 0.f};
  relativeCenter = glm::rotate(relativeCenter, angle, {0.f, 0.f, 1.f});
  return position + glm::vec2(relativeCenter);
}

void Player::giveWeapon(const Weapon& n)
{
  // Do nothing if player already has weapon of type
  for (const auto& w : weapons) {
    if (w.type == n.type) return;
  }

  // Add weapon - sort weapon list by weapon type
  weapons.push_back(n);
  std::sort(weapons.begin(), weapons.end(),
      [](const Weapon& w, const Weapon& w2) -> bool {
      return w.type < w2.type;
      });
}
