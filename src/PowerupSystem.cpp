#include "PowerupSystem.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

#include "Random.hpp"
#include "Terrain.hpp"
#include "EventManager.hpp"

PowerupSystem::PowerupSystem(const Terrain* t) :
  terrain(t)
{
  spawnPowerup();
}

void PowerupSystem::update(float dt)
{
  for (auto& p : powerups) {
    if (!p.landed) {
      float speed = 2000.f;
      p.position.y -= speed * dt * glm::sin(p.angle);
      p.position.x -= speed * dt * glm::cos(p.angle);

      if (p.position.y < terrain->getHeight(p.position.x)) {
	p.landed = true;
	
	Event e{Event::EXPLOSION};
	e.data.push_back(p.position);
	e.data.push_back(100.f);
	EventManager::Send(e);
      }
    }

    if (p.landed) {
      if (p.position.y != terrain->getHeight(p.position.x))
	p.position.y = terrain->getHeight(p.position.x);
    }

  }
}

void PowerupSystem::spawnPowerup()
{
  Powerup p;
  p.landed = false;

  p.targetPosition.x = 400.f;
  p.targetPosition.y = terrain->getHeight(p.targetPosition.x);
  
  p.angle = glm::half_pi<float>() +
    Random::randomFloat(-glm::quarter_pi<float>(), glm::quarter_pi<float>());

  p.position = p.targetPosition;
  p.position.x += 2000.f * glm::cos(p.angle);
  p.position.y += 2000.f * glm::sin(p.angle);

  powerups.push_back(p);
}
