#include "PlayerSystem.hpp"

#include <glm/glm.hpp>

#include "EventManager.hpp"
#include "Random.hpp"
#include "Player.hpp"
#include "Grenade.hpp"
#include "Terrain.hpp"
#include "TimescaleSystem.hpp"
#include "Powerup.hpp"
#include "Joystick.hpp"
#include "geo.hpp"

#include "imgui.h"
#include "Console.hpp"

#include <glm/gtc/constants.hpp>

#include <iostream>

PlayerSystem::PlayerSystem(
    const Terrain& t,
    const std::map<int, ControllerData>& c,
    const TimescaleSystem& ts) :
  terrain(t),
  controllers(c),
  timescaleSystem(ts)
{
  for (const auto& c : controllers) {
    players.emplace_back(Player());
    Player& player = players.back();

    player.id = players.size()-1;
    player.position.x = 2000.f;
    player.controllerID = c.first;

    player.giveGrenade(Grenade::Type::STANDARD, 5);
    player.giveGrenade(Grenade::Type::CLUSTER, 8);
    player.giveGrenade(Grenade::Type::INERTIA, 10);
    player.giveGrenade(Grenade::Type::TELEPORT, 10);
  }

  // Create dummy player
  players.emplace_back(Player());
  Player& player = players.back();

  player.id = players.size()-1;
  player.position.x = 2200.f;
  player.controllerID = -1;

  EventManager::Register(Event::EXPLOSION,
      std::bind(&PlayerSystem::onExplosion, this, _1));

  EventManager::Register(Event::POWERUP_PICKUP,
      std::bind(&PlayerSystem::onPowerupPickup, this, _1));
}

void PlayerSystem::update(double t, double gdt)
{
  for (auto& p : players) {
    double newTimescale = timescaleSystem.getTimescaleAtPosition(p.position);
    double inertia = newTimescale < p.localTimescale ? 0.3 : 0.1;
    p.localTimescale += inertia *
      (timescaleSystem.getTimescaleAtPosition(p.position) - p.localTimescale);
    double dt = p.localTimescale * gdt;

    std::vector<float> axes(6, 0.0f);

    if (p.controllerID != -1) {
      axes = controllers.at(p.controllerID).axes;
    }

    // Aiming
    ////////////////////////////////////////////////////

    // Deadzone
    if (pow(axes[0], 2) + pow(axes[1], 2)
	< pow(Player::MOVEMENT_DEADZONE, 2)) {
      axes[0] = 0.f;
      axes[1] = 0.f;
    } else {
      p.lastMovingRight = axes[0] > 0.f;
    }

    float aimSpeed;
    if (!p.firingBeam) aimSpeed = Player::AIM_SPEED;
    else aimSpeed = Player::AIM_SPEED_BEAM;

    float currentAngle = p.aimDirection;
    float targetAngle;
    if (axes[0] == 0.f && axes[1] == 0.f) {
      targetAngle = -p.angle +
	(int)!p.lastMovingRight * glm::pi<float>();
    } else {
      targetAngle = glm::atan(axes[1], axes[0]);
    }

    float angleDiff = fabs(targetAngle - currentAngle);
    if (angleDiff > glm::pi<float>()) {
      if (targetAngle > currentAngle) currentAngle += glm::two_pi<float>();
      else targetAngle += glm::two_pi<float>();
    }
    float newAngle = currentAngle +
      (aimSpeed * dt * (targetAngle - currentAngle));

    if (newAngle >= 0.f && newAngle < glm::two_pi<float>())
      p.aimDirection = newAngle;
    else
      p.aimDirection = fmod(newAngle, glm::two_pi<float>());

    // Movement
    ////////////////////////////////////////////////////

    // -------- Acceleration --------
    float accel_factor = Player::ACCEL_X;
    if (p.airborne)
      accel_factor = Player::ACCEL_X_AIRBORNE;
    if (p.airborne && (p.outOfControl || p.firingBeam))
      accel_factor = Player::ACCEL_X_AIRBORNE_NOCONTROL;
    if (!p.airborne && (p.outOfControl || p.firingBeam))
      accel_factor = Player::ACCEL_X_NOCONTROL;

    // Acceleration due to player input
    if (!p.outOfControl && !p.firingBeam) {
      p.acceleration.x = axes[0] * accel_factor;
    } else {
      p.acceleration.x = 0.f;
    }

    // Drag to oppose velocity
    p.acceleration.x -= accel_factor / Player::MAX_SPEED * p.velocity.x;

    // Gravity
    if (p.airborne) {
      p.acceleration.y = Player::ACCEL_Y;
    }

    // -------- Velocity --------
    p.velocity += p.acceleration * (float)dt;
    glm::vec2 maxNewPosition = p.position + p.velocity * (float)dt;

    float terrainAngle = terrain.getAngle(maxNewPosition.x);
    if (!p.airborne &&
	glm::sign(terrainAngle) == glm::sign(p.velocity.x)) {
      p.velocity *= glm::cos(terrainAngle);
    }

    // -------- Position --------
    glm::vec2 newPosition = p.position + p.velocity * (float)dt;

    if (!p.airborne) {
      if (abs(terrainAngle) > Player::MAX_DOWNHILL_ANGLE &&
	  glm::sign(p.velocity.x) != glm::sign(terrainAngle)) {
	p.dirty_justLeftGround = true;
	p.airborne = true;
      }
      else {
	p.position = newPosition;
	p.position.y = terrain.getHeight(newPosition.x);
      }
    }

    if (p.airborne) {
      if (!p.dirty_justLeftGround) {
	auto intersection = terrain.intersect(p.position, newPosition);
	if (intersection.first) {
	  p.acceleration.y = 0.f;
	  p.velocity.y = 0.f;
	  p.airborne = false;
	  p.outOfControl = false;
	  p.jumpAvailable = true;
	  break;
	}

	// Failsafe
	if (!intersection.first && newPosition.y <
	    terrain.getHeight(newPosition.x))
	  newPosition.y = terrain.getHeight(newPosition.x);
      } else {
	p.dirty_justLeftGround = false;
      }

      p.position = newPosition;
    }

    // World boundary
    float terrainMaxWidth = terrain.getMaxWidth();
    if (p.position.x - Player::SIZE < 0)
      p.position.x = Player::SIZE;
    else if (p.position.x + Player::SIZE > terrainMaxWidth)
      p.position.x = terrainMaxWidth - Player::SIZE;

    // -------- Angle --------
    float goalAngle = terrainAngle;

    float terrainHeight = terrain.getHeight(p.position.x);
    float heightModifier = (p.position.y - terrainHeight) / 220.f;
    if (heightModifier < 0.f) heightModifier = 0.f;
    if (heightModifier > 1.f) heightModifier = 1.f;

    float velocityModifier = p.velocity.x / Player::MAX_SPEED;
    if (velocityModifier > 1.0f) velocityModifier = 1.0f;

    // Slightly tilt towards velocity direction
    goalAngle += heightModifier *
      (-glm::radians(15.f) * velocityModifier - goalAngle);

    p.angle += 14.f * dt * (goalAngle - p.angle);

    // Health
    if (p.health <= 0.f) {
      p.health = 0.f;

      if (p.alive) {
	p.alive = false;

	EvdPlayerDeath d;
	d.player = &p;
	EventManager::Send(Event::PLAYER_DEATH, d);

	Console::log() << red << "Player death";
      }
    }
  }

  ImGui::Begin("Players", NULL, ImGuiWindowFlags_NoCollapse);
  for (auto& p : players) {
    std::stringstream header_label;
    header_label << "Player " << p.id; 

    int flags = ImGuiTreeNodeFlags_DefaultOpen;

    if(ImGui::CollapsingHeader(header_label.str().c_str(), flags)) {
      ImGui::Text("Health: %2.f", p.health);
      if (p.inventory.size()) {
	ImGui::Text("---- Primary ----");
	ImGui::Text("%s (%i)",
	    Grenade::getTypeString(p.inventory[p.primaryGrenadeSlot].type),
	    p.inventory[p.primaryGrenadeSlot].ammo);
	ImGui::Text("---- Secondary (%i) ----", p.combinationEnabled);
	if (p.combinationEnabled) {
	ImGui::Text("%s (%i)",
	    Grenade::getTypeString(p.inventory[p.secondaryGrenadeSlot].type),
	    p.inventory[p.secondaryGrenadeSlot].ammo);
	} else {
	  ImGui::Text("");
	}
      }
    }
  }
  ImGui::End();
}

void PlayerSystem::processInput(int controllerID, int button, bool action)
{
  auto it = std::find_if(players.begin(), players.end(),
      [controllerID](const Player& p) -> bool {
      return p.controllerID == controllerID;
      });

  Player& player = *it;

  // Press
  if (action) {
    switch (button) {
      case JOY_BUTTON_A:
	jump(player);
	break;

      case JOY_BUTTON_RB:
	player.primingGrenade = true;
	break;

      case JOY_BUTTON_Y:
	cycleGrenade(player);
	break;

      case JOY_BUTTON_LB:
	detonateGrenade(player);

      default: break;
    }
  }

  // Release
  else {
    switch (button) {
      case JOY_BUTTON_RB:
	player.primingGrenade = false;
	throwGrenade(player);
	break;
      default: break;
    }
  }
}

void PlayerSystem::jump(Player& p)
{
  if (!p.jumpAvailable || p.outOfControl || p.firingBeam) return;

  float terrainAngle = terrain.getAngle(p.position.x);

  p.velocity.y = Player::JUMP_VELOCITY;
  if (abs(terrainAngle) > Player::MIN_SIDEJUMP_ANGLE) {
    p.velocity.x += 0.4f * Player::JUMP_VELOCITY * -glm::sin(terrainAngle); 
  }
  p.dirty_justLeftGround = true;
  p.airborne = true;
  p.jumpAvailable = false;
}

void PlayerSystem::throwGrenade(Player& p)
{
  EvdPlayerThrowGrenade d;
  d.player = &p;
  EventManager::Send(Event::PLAYER_THROW_GRENADE, d);

  p.combinationEnabled = false;
  p.secondaryGrenadeSlot = 0;
}

void PlayerSystem::detonateGrenade(Player& p)
{
  EvdPlayerDetonateGrenade d;
  d.player = &p;
  EventManager::Send(Event::PLAYER_DETONATE_GRENADE, d);
}

void PlayerSystem::cycleGrenade(Player& p)
{
  if (p.inventory.size() <= 1) return;

  //// Primary grenade slot
  if (!p.primingGrenade) {
    p.primaryGrenadeSlot = (p.primaryGrenadeSlot+1) % p.inventory.size();
  }
  //// Secondary grenade slot
  else {
    // If combination is disabled, enable it, and reset the secondary slot
    if (!p.combinationEnabled) {
      p.combinationEnabled = true;
      p.secondaryGrenadeSlot = p.primaryGrenadeSlot ? 0 : 1;
    }
    // If it's already enabled, increment the slot
    else {
      auto oldSlot = p.secondaryGrenadeSlot;
      p.secondaryGrenadeSlot =
	(p.secondaryGrenadeSlot+1) % p.inventory.size();

      // But don't allow the slot to match the primary slot
      // (can't combine the same grenade with itself)
      if (p.secondaryGrenadeSlot == p.primaryGrenadeSlot) {
	p.secondaryGrenadeSlot =
	  (p.secondaryGrenadeSlot+1) % p.inventory.size();
      }

      if (p.secondaryGrenadeSlot < oldSlot)
	p.combinationEnabled = false;
    }
  }
}

void PlayerSystem::onExplosion(const Event& e)
{
  const auto* g = boost::any_cast<EvdGrenadeExplosion>(e.data).grenade;

  for (auto& p : players) {

    // Teleport
    if (g->owner == p.id &&
	g->properties.teleportPlayerOnDetonate) {
      p.position = g->position;
      p.airborne = true;
      p.jumpAvailable = false;

      if (p.velocity.y < 0) p.velocity.y = 0;
    }

    glm::vec2 diff = p.position - g->position;
    float dist = glm::length(diff);

    if (dist >= g->properties.radius) continue;

    // Damage falloff
    float damage = g->properties.damage * 
      (1 - glm::pow((dist / g->properties.radius), 0.74f));

    if (damage < 0.1*g->properties.damage)
      damage = 0.1 * g->properties.damage;

    p.health -= damage;

    p.airborne = true;
    p.dirty_justLeftGround = true;
    p.outOfControl = true;

    glm::vec2 launchVelocity;

    launchVelocity.x = g->properties.knockback * 
      glm::pow( (g->properties.radius-dist)/g->properties.radius, 0.5);
    launchVelocity.x *= glm::sign(diff.x);

    launchVelocity.y = g->properties.knockback *
      glm::pow( (g->properties.radius-dist)/g->properties.radius, 0.5);

    // If we are very close in x, launch more upwards
    if (glm::abs(diff.x) < 1.5f * Player::SIZE) {
      launchVelocity.x *= 0.8f;
      launchVelocity.y *= 1.5f;
    }

    p.velocity += launchVelocity;
  }
}

void PlayerSystem::onPowerupPickup(const Event& e)
{
  auto d = boost::any_cast<EvdPowerupPickup>(e.data);

  Player& p = players[d.playerID];
}
