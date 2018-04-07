#include "PowerupSystem.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include "Random.hpp"
#include "Terrain.hpp"
#include "PlayerSystem.hpp"
#include "EventManager.hpp"

#include <iostream>

PowerupSystem::PowerupSystem(const Terrain& t, const PlayerSystem& p) :
  terrain(t),
  playerSystem(p)
{
  EventManager::Register(Event::GAME_START, [this](Event) {
      this->spawnPowerup();
      });
}

void PowerupSystem::update(double dt)
{
  // Remove powerups awaiting removal
  powerups.erase(std::remove_if(powerups.begin(), powerups.end(),
	[](const Powerup& p) -> bool {
	return p.dirty_awaitingRemoval;
	}), powerups.end());
  
  for (auto& p : powerups) {
    if (!p.landed) {
      float speed = 1500.f;
      glm::vec2 newPosition;
      newPosition.x = p.position.x - speed * dt * glm::sin(p.angle);
      newPosition.y = p.position.y - speed * dt * glm::cos(p.angle);

      auto intersection = terrain.intersect(p.position, newPosition);
      if(intersection.first) {
	p.landed = true;
	p.position = intersection.second;
      } else {
	p.position = newPosition;
      }
    }

    if (p.landed) {
      if (p.position.y != terrain.getHeight(p.position.x))
	p.position.y = terrain.getHeight(p.position.x);

      // Check if any player is in pickup range
      int playerID = -1;
      for (const auto& a : playerSystem.getPlayers()) {
	float dx = glm::distance(a.position, p.position);
	if (dx < 16.f) {
	  playerID = a.id;
	  break;
	}
      }

      // Player picked up powerup
      if (playerID != -1) {
	EvdPowerupPickup d;
	d.powerupType = p.type;
	d.playerID = playerID;
	EventManager::Send(Event::POWERUP_PICKUP, d);
	p.dirty_awaitingRemoval = true;
	continue;
      }
    }
  }
}

void PowerupSystem::spawnPowerup()
{
  Powerup p;
  p.landed = false;

  p.targetPosition.x = 1950.f;
  p.targetPosition.y = terrain.getHeight(p.targetPosition.x);

  p.angle =
    Random::randomFloat(-glm::quarter_pi<float>(), glm::quarter_pi<float>());

  p.position = p.targetPosition;
  p.position.x += 2000.f * glm::sin(p.angle);
  p.position.y += 2000.f * glm::cos(p.angle);

  p.type = Powerup::Type::TEST;

  powerups.push_back(p);
}
