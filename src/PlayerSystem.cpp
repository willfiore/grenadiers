#include "PlayerSystem.hpp"

#include <glm/glm.hpp>

#include "Random.hpp"
#include "Player.hpp"
#include "Terrain.hpp"
#include "Powerup.hpp"
#include "Joystick.hpp"
#include "geo.hpp"

#include "imgui.h"
#include "Console.hpp"

#include <glm/gtc/constants.hpp>

#include <iostream>

PlayerSystem::PlayerSystem(
    const Terrain* t,
    const std::map<int, ControllerData>* c) :
  terrain(t),
  controllers(c)
{
  for (const auto& c : *controllers) {
    Player player;
    player.position.x = 2000.f + Random::randomFloat(0.f, 200.f);
    player.weapons.insert({
	Weapon::GRENADE,
	Weapon::MISSILE,
	Weapon::BEAM
	});

    player.id = players.size();
    player.controllerID = c.first;

    players.push_back(player);
  }
  
  // Create dummy player
  Player player;
  player.position.x = 2200.f;
  player.id = players.size();
  player.controllerID = -1;
  players.push_back(player);

  EventManager::Register(Event::EXPLOSION,
      std::bind(&PlayerSystem::onExplosion, this, _1));
  
  EventManager::Register(Event::POWERUP_PICKUP,
      std::bind(&PlayerSystem::onPowerupPickup, this, _1));
}

void PlayerSystem::update(float dt)
{
  for (auto& p : players) {

    std::vector<float> axes(6, 0.0f);

    if (p.controllerID != -1) {
      axes = controllers->at(p.controllerID).axes;
    }

    // Aiming
    ////////////////////////////////////////////////////
    p.aimDirection = glm::atan(axes[1], axes[0]);

    // Movement
    ////////////////////////////////////////////////////

    // -------- Acceleration --------
    float accel_factor = Player::ACCEL_X;
    if (p.airborne)
      accel_factor = Player::ACCEL_X_AIRBORNE;
    if (p.airborne && p.outOfControl)
      accel_factor = Player::ACCEL_X_NOCONTROL;

    // Acceleration due to player input
    if (!p.outOfControl) {
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
    p.velocity += p.acceleration * dt;
    glm::vec2 maxNewPosition = p.position + p.velocity * dt;

    float terrainAngle = terrain->getAngle(maxNewPosition.x);
    if (!p.airborne &&
	glm::sign(terrainAngle) == glm::sign(p.velocity.x)) {
      p.velocity *= glm::cos(terrainAngle);
    }

    // -------- Position --------
    glm::vec2 newPosition = p.position + p.velocity * dt;

    if (!p.airborne) {
      if (abs(terrainAngle) > Player::MAX_DOWNHILL_ANGLE &&
	  glm::sign(p.velocity.x) != glm::sign(terrainAngle)) {
	p.dirty_justLeftGround = true;
	p.airborne = true;
      }
      else {
	p.position = newPosition;
	p.position.y = terrain->getHeight(newPosition.x);
      }
    }

    if (p.airborne) {
      if (!p.dirty_justLeftGround) {
	std::vector<LineSegment> tSegments =
	  terrain->getSegmentsInRange(p.position.x, newPosition.x);

	bool foundIntersection = false;
	for (auto& s : tSegments) {

	  auto intersection =
	    geo::intersect(s.first, s.second, p.position, newPosition);

	  if (intersection.first) {
	    foundIntersection = true;

	    newPosition = intersection.second;
	    p.acceleration.y = 0.f;
	    p.velocity.y = 0.f;
	    p.airborne = false;
	    p.outOfControl = false;
	    p.jumpAvailable = true;
	    break;
	  }
	}

	// Failsafe
	if (!foundIntersection && newPosition.y < terrain->getHeight(newPosition.x))
	  newPosition.y = terrain->getHeight(newPosition.x);
      } else {
	p.dirty_justLeftGround = false;
      }

      p.position = newPosition;
    }

    // World boundary
    float terrainMaxWidth = terrain->getMaxWidth();
    if (p.position.x - Player::SIZE < 0)
      p.position.x = Player::SIZE;
    else if (p.position.x + Player::SIZE > terrainMaxWidth)
      p.position.x = terrainMaxWidth - Player::SIZE;

    // -------- Angle --------
    float goalAngle = terrainAngle;

    float terrainHeight = terrain->getHeight(p.position.x);
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
    if (p.health <= 0.f && p.alive) {
      p.health = 0.f;
      p.alive = false;

      EvdPlayerDeath d;
      d.player = p;
      EventManager::Send(Event::PLAYER_DEATH, d);

      Console::log() << red << "Player death";
    }

  }

  ImGui::Begin("Players", NULL, ImGuiWindowFlags_NoCollapse);
  for (auto& p : players) {
    std::stringstream header_label;
    header_label << "Player " << p.id; 

    int flags = 0;
    if (p.id == 0) flags = ImGuiTreeNodeFlags_DefaultOpen;

    if(ImGui::CollapsingHeader(header_label.str().c_str(), flags)) {
      ImGui::Text("Health: %2.f", p.health);
      ImGui::Text("Weapon: %lu", p.currentWeaponIndex);
      ImGui::Text("Airborne: %i", p.airborne);
      ImGui::Text("Jump available: %i", p.jumpAvailable);
      ImGui::Text("Out of control: %i", p.outOfControl);
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
	jump(player); break;
      case JOY_BUTTON_RB:
	fireWeapon(player); break;
      case JOY_BUTTON_LB:
	secondaryFireWeapon(player); break;
      case JOY_BUTTON_Y:
	cycleWeapon(player); break;
      default: break;
    }
  }

  // Release
  else {
    switch (button) {
      case JOY_BUTTON_RB:
	releaseWeapon(player); break;
      default: break;
    }
  }
}

void PlayerSystem::jump(Player& p)
{
  if (!p.jumpAvailable || p.outOfControl) return;

  float terrainAngle = terrain->getAngle(p.position.x);

  p.velocity.y = Player::JUMP_VELOCITY;
  if (abs(terrainAngle) > Player::MIN_SIDEJUMP_ANGLE) {
    p.velocity.x += 0.4f * Player::JUMP_VELOCITY * -glm::sin(terrainAngle); 
  }
  p.dirty_justLeftGround = true;
  p.airborne = true;
  p.jumpAvailable = false;
}

void PlayerSystem::fireWeapon(Player& p)
{
  // Can't fire if they have no weapons!
  if (p.weapons.size() == 0) return;

  Weapon currentWeapon = *std::next(p.weapons.begin(), p.currentWeaponIndex);

  if (currentWeapon == Weapon::BEAM) {
    p.firingBeam = true;
  }

  EvdPlayerFireWeapon d;
  d.player = p;
  d.weapon = currentWeapon;
  EventManager::Send(Event::PLAYER_FIRE_WEAPON, d);
}

void PlayerSystem::releaseWeapon(Player& p)
{
  if (p.weapons.size() == 0) return;

  Weapon currentWeapon = *std::next(p.weapons.begin(), p.currentWeaponIndex);

  if (currentWeapon == Weapon::BEAM) {
    p.firingBeam = false;
  }

  EventManager::Send(Event::PLAYER_RELEASE_WEAPON);
}

void PlayerSystem::secondaryFireWeapon(Player& p)
{
  // Can't fire if they have no weapons!
  if (p.weapons.size() == 0) return;

  Weapon currentWeapon = *std::next(p.weapons.begin(), p.currentWeaponIndex);

  EvdPlayerSecondaryFireWeapon d;
  d.player = p;
  d.weapon = currentWeapon;
  EventManager::Send(Event::PLAYER_SECONDARY_FIRE_WEAPON, d);
}

void PlayerSystem::cycleWeapon(Player& p)
{
  if (p.weapons.size() <= 1) return;

  p.currentWeaponIndex++;

  // Wrap
  if(p.currentWeaponIndex >= p.weapons.size()) {
    p.currentWeaponIndex = 0;
  }
}

void PlayerSystem::onExplosion(Event e)
{
  auto d = boost::any_cast<EvdExplosion>(e.data);

  for (auto& p : players) {
    glm::vec2 diff = p.position - d.position;
    float dist = glm::length(diff);

    if (dist > d.radius) continue;

    // Damage falloff
    float damage = d.damage * (1 - glm::pow((dist / d.radius), 0.74f));

    if (damage < 0.1*d.damage)
      damage = 0.1 * d.damage;

    p.health -= damage;

    p.airborne = true;
    p.dirty_justLeftGround = true;
    p.outOfControl = true;

    glm::vec2 launchVelocity;

    launchVelocity.x = d.knockback * glm::pow( (d.radius-dist)/d.radius, 0.5);
    launchVelocity.x *= glm::sign(diff.x);

    launchVelocity.y = d.knockback * glm::pow( (d.radius-dist)/d.radius, 0.5);

    // If we are very close in x, launch more upwards
    if (glm::abs(diff.x) < 1.5f * Player::SIZE) {
      launchVelocity.x *= 0.8f;
      launchVelocity.y *= 1.5f;
    }

    p.velocity += launchVelocity;
  }
}

void PlayerSystem::onPowerupPickup(Event e)
{
  auto d = boost::any_cast<EvdPowerupPickup>(e.data);

  Player& p = players[d.playerID];
}
