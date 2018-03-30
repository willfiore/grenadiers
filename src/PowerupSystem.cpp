#include "PowerupSystem.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include "Random.hpp"
#include "Terrain.hpp"
#include "PlayerSystem.hpp"
#include "EventManager.hpp"

#include <iostream>

PowerupSystem::PowerupSystem(const Terrain* t, const PlayerSystem* p) :
  terrain(t),
  playerSystem(p)
{
  EventManager::Register(Event::GAME_START, [this](Event e) {
      this->spawnPowerup();
      });
}

void PowerupSystem::update(float dt)
{
  for (auto it = powerups.begin(); it != powerups.end();) {
    auto& p = *it;

    if (!p.landed) {
      float speed = 1500.f;
      p.position.x -= speed * dt * glm::sin(p.angle);
      p.position.y -= speed * dt * glm::cos(p.angle);

      if (p.position.y < terrain->getHeight(p.position.x)) {
	p.landed = true;
	
	Event e{Event::EXPLOSION};
	e << p.position << 100.f;
	EventManager::Send(e);
      }
    }

    if (p.landed) {
      if (p.position.y != terrain->getHeight(p.position.x))
	p.position.y = terrain->getHeight(p.position.x);

      // Check if any player is in pickup range
      int playerID = -1;
      for (const auto& a : playerSystem->getPlayers()) {
	float dx = glm::distance(a.position, p.position);
	if (dx < 16.f) {
	  playerID = a.id;
	  break;
	}
      }

      // Player picked up powerup
      if (playerID != -1) {
	Event e{Event::POWERUP_PICKUP};
	e << p.type << playerID;
	EventManager::Send(e);
	powerups.erase(it);
	continue;
      }
    }

    ++it;
  }
}

void PowerupSystem::spawnPowerup()
{
  Powerup p;
  p.landed = false;

  p.targetPosition.x = 600.f;
  p.targetPosition.y = terrain->getHeight(p.targetPosition.x);

  p.angle =
    Random::randomFloat(-glm::quarter_pi<float>(), glm::quarter_pi<float>());

  p.position = p.targetPosition;
  p.position.x += 2000.f * glm::sin(p.angle);
  p.position.y += 2000.f * glm::cos(p.angle);

  p.type = Powerup::Type::TEST;

  powerups.push_back(p);
}
