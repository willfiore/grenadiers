#pragma once

#include <glm/matrix.hpp>
#include <glm/vec2.hpp>

class PlayerSystem;

class CameraSystem
{
public:
  CameraSystem(const PlayerSystem*);
  void update(float dt);
  void setWindowDimensions(int, int);

  glm::mat4 getView() const;

private:
  glm::vec2 position;
  glm::vec2 rotation;

  int windowWidth;
  int windowHeight;
  
  const PlayerSystem* playerSystem;
};
