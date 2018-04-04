#pragma once

#include <vector>
#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

#include "Event.hpp"

class Window;
struct Player;

class CameraSystem
{
public:
  CameraSystem(const Window*, const std::vector<Player>&);
  void update(double t, double dt);

  glm::mat4 getView() const;
  glm::mat4 getProjection() const;

private:
  glm::vec3 position;
  glm::vec2 rotation;

  const Window* window;
  
  float fov;

  float shakeAmplitude;
  float shakeStartTimestamp;
  
  const std::vector<Player>& players;

  void onExplosion(Event);
};
