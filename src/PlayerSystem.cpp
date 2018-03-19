#include "PlayerSystem.hpp"

#include <glm/trigonometric.hpp>

#include "Player.hpp"
#include "Terrain.hpp"
#include "Joystick.hpp"
#include "ProjectileSystem.hpp"

#include <iostream>

PlayerSystem::PlayerSystem(const Terrain* t, ProjectileSystem* p) :
  terrain(t),
  projectileSystem(p)
{
  players.emplace_back(Player());

  EventManager::Register(Event::EXPLOSION,
      std::bind(&PlayerSystem::onExplosion, this, _1));
}

void PlayerSystem::update(float dt, const float* axes)
{
  for (auto& p : players) {

    // Movement
    ////////////////////////////////////////////////////

    // Gravity
    p.velocity.y -= dt * Player::FALL_ACCEL;

    // Axes movement
    if (!p.outOfControl) {
      p.velocity.x = Player::SPEED * axes[0];
    }
    
    p.position += p.velocity * dt;

    p.aimDirection = glm::atan(axes[1], axes[0]);

    // Lock to terrain if not airborne
    float terrainHeight = terrain->getHeight(p.position.x);

    if (p.position.y <= terrainHeight || !p.airborne) {
      p.velocity.y = 0;
      p.position.y = terrainHeight;

      p.airborne = false;
      p.outOfControl = false;
      p.doubleJumpAvailable = false;
    }

    if (p.position.x - Player::SIZE < 0)
      p.position.x = Player::SIZE;
    if (p.position.x + Player::SIZE > terrain->getWidth())
      p.position.x = terrain->getWidth() - Player::SIZE;

    // Rotation
    ////////////////////////////////////////////////////
    float angleHeightModifier =
      (150.f - p.position.y + terrainHeight) / 150.f;

    if (angleHeightModifier < 0) angleHeightModifier = 0;

    p.goalAngle = terrain->getAngle(p.position.x);
    p.goalAngle *= angleHeightModifier;

    if (p.outOfControl) {
      p.goalAngle += 0.2f * -glm::sign(p.velocity.x);
    }

    p.angularVelocity = (p.goalAngle - p.angle) * Player::ANGLE_ACCEL_FACTOR;
    p.angle += p.angularVelocity * dt;
  }
}

void PlayerSystem::processInput(int playerID, int button, bool action)
{
  Player& player = players[playerID];

  // Press
  if (action) {
    switch (button) {
      case JOY_BUTTON_A: jump(player); break;
      case JOY_BUTTON_RB: launchGrenade(player); break;
      default: break;
    }
  }

  // Release
  else {
    switch (button) {
      default: break;
    }
  }
}

void PlayerSystem::jump(Player& p)
{
  if (p.outOfControl) return;
  if (p.airborne && !p.doubleJumpAvailable) return;
  p.airborne = true;

  if (!p.doubleJumpAvailable) {
    p.doubleJumpAvailable = true;
    p.velocity.y = Player::JUMP_SPEED;
  } else {
    p.doubleJumpAvailable = false;
    p.outOfControl = false;
    p.velocity.y = Player::JUMP_SPEED * Player::DOUBLE_JUMP_MULTIPLIER;
  }
}

void PlayerSystem::launchGrenade(Player& p)
{
  // Inherit player velocity
  glm::vec2 velocity = p.velocity / 1.5f;

  float strength = 400.f;
  velocity.x += strength * glm::cos(p.aimDirection);
  velocity.y += strength * -glm::sin(p.aimDirection);

  projectileSystem->spawnGrenade(
      p.position + glm::vec2(0.f, Player::SIZE), velocity);
}

void PlayerSystem::onExplosion(Event e)
{
  for (auto& p : players) {
    glm::vec2 diff = p.position - e.position;
    float dist = glm::length(diff);

    if (dist < e.radius) {
      p.airborne = true;
      p.outOfControl = true;

      glm::vec2 launchVelocity;

      launchVelocity.x = 1000.f * glm::pow( (e.radius-dist)/e.radius, 0.5);
      launchVelocity.x *= glm::sign(diff.x);

      launchVelocity.y = 1000.f * glm::pow( (e.radius-dist)/e.radius, 0.5);

      // If we are very close in x, launch more upwards
      if (glm::abs(diff.x) < 1.5f * Player::SIZE) {
	launchVelocity.x *= 0.2f;
	launchVelocity.y *= 1.5f;
      }

      p.velocity += launchVelocity;
    }
  }
}
