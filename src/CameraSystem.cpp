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
  position.y = 0;

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
  view = glm::translate(view, glm::vec3(windowWidth/2, 0, 0));
  view = glm::translate(view,
      glm::vec3(-position.x, position.y, 0.f));

  return view;
}

void CameraSystem::onExplosion(Event e)
{
  glm::vec2 explosionPosition = boost::get<glm::vec2>(e.data[0]);
  float distance = glm::distance(explosionPosition, position);
  shake = 25.f - (pow(distance/50.f, 2));
  if (shake < 2.f) shake = 2.f;
}
