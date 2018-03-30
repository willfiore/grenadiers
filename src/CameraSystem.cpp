#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Random.hpp"
#include "EventManager.hpp"
#include "PlayerSystem.hpp"
#include "ResourceManager.hpp"

#include <numeric>

CameraSystem::CameraSystem(const std::vector<Player>& p) :
  players(p)
{
  fov = 60.f;
  shake = 1.f;
}

void CameraSystem::update(float dt)
{
  if (players.size() == 0) return;

  glm::vec2 minPlayerBounds{
    std::numeric_limits<float>::infinity(),
      std::numeric_limits<float>::infinity()
  };
  glm::vec2 maxPlayerBounds{
    -std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity()
  };

  glm::vec2 averagePlayerPos;

  for (size_t i = 0; i < players.size(); ++i) {
    const Player& p = players[i];

    averagePlayerPos += p.position;

    if (p.position.x < minPlayerBounds.x) minPlayerBounds.x = p.position.x;
    if (p.position.y < minPlayerBounds.y) minPlayerBounds.y = p.position.y;
    if (p.position.x > maxPlayerBounds.x) maxPlayerBounds.x = p.position.x;
    if (p.position.y > maxPlayerBounds.y) maxPlayerBounds.y = p.position.y;
  }

  averagePlayerPos /= players.size();

  float maxRange = maxPlayerBounds.x - minPlayerBounds.x;

  glm::vec3 targetPos = {averagePlayerPos.x, 100.f + 0.8f*minPlayerBounds.y, 350.f};

  float rangeModifier = 0.5f * (maxRange - 200.f);
  if (rangeModifier < 0.f) rangeModifier = 0.f;
  if (rangeModifier > 400.f) rangeModifier = 400.f;

  targetPos.z += rangeModifier;

  position = position + (targetPos - position) * dt * 4.f;

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
      position,
      glm::vec3(position.x, position.y, 0.f),
      glm::vec3(0.f, 1.f, 0.f));

  return view;
}

glm::mat4 CameraSystem::getProjection() const
{
  glm::mat4 projection = glm::perspective(
      glm::radians(fov),
      (float)windowWidth / (float)windowHeight,
      0.01f, 10000.f
      );

  return projection;
}

void CameraSystem::onExplosion(Event e)
{
  shake = 12.f;
}
