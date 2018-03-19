#include "CameraSystem.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "PlayerSystem.hpp"

CameraSystem::CameraSystem(const PlayerSystem* p) :
  playerSystem(p)
{
}

void CameraSystem::update(float dt)
{
  const Player& p = playerSystem->getPlayers().front();
  position.x = position.x + (- p.position.x - position.x) * dt * 4.f;
  position.y = 0;
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
      glm::vec3(position.x, position.y, 0.f));

  return view;
}
