#pragma once

#include <vector>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

#include "Event.hpp"

class Player;

class CameraSystem
{
public:
  CameraSystem(const std::vector<Player>&);
  void update(float dt);
  void setWindowDimensions(int, int);

  glm::mat4 getView() const;
  glm::mat4 getProjection() const;

private:
  glm::vec3 position;
  glm::vec2 rotation;

  int windowWidth;
  int windowHeight;
  
  float fov;
  float shake;
  
  const std::vector<Player>& players;

  void onExplosion(Event);
};
