#pragma once

#include <vector>

#include "BaseRenderer.hpp"

class PlayerSystem;
struct Player;

class PlayerRenderer : public BaseRenderer
{
public:
  PlayerRenderer(const PlayerSystem*);
  virtual void draw() override;
private:
  const PlayerSystem* playerSystem;
  Shader shader;
};
