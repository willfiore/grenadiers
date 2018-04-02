#include "Player.hpp"

#include <vector>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

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

glm::vec2 Player::getCenterPosition()
{
  return position + glm::vec2(0.f, Player::SIZE);
}
