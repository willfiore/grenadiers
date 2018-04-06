#include "Player.hpp"

#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

  // Init state
  health = STARTING_HEALTH;
  grenadeRefreshTimer = 0.0;

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
