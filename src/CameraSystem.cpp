#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Random.hpp"
#include "EventManager.hpp"
#include "PlayerSystem.hpp"

CameraSystem::CameraSystem(const PlayerSystem* p) :
  playerSystem(p)
{
}

void CameraSystem::update(float dt)
{
  const Player& p = playerSystem->getPlayers().front();
  position.x = position.x + (p.position.x - position.x) * dt * 4.f;
  position.y = 100.f;

  shake = shake * 0.87f;
  if (shake < 0.f) shake = 0.f;

  position.x += Random::randomFloat(-shake, shake);
  position.y += Random::randomFloat(-shake, shake);

  EventManager::Register(Event::EXPLOSION,
      std::bind(&CameraSystem::onExplosion, this, _1));
}

void CameraSystem::setWindowDimensions(int w, int h)
{
  windowWidth = w;
  windowHeight = h;
}

glm::mat4 CameraSystem::getView() const
{
  glm::mat4 view;
  view = glm::lookAt(
      glm::vec3(position.x, position.y, 400.f),
      glm::vec3(position.x, position.y, 0.f),
      glm::vec3(0.f, 1.f, 0.f));

  return view;
}

void CameraSystem::onExplosion(Event e)
{
  shake = 12.f;
}
