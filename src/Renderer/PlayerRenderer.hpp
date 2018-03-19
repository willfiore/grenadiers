#pragma once

#include <vector>

#include "BaseRenderer.hpp"
#include "../PlayerSystem.hpp"

class Player;

class PlayerRenderer : public BaseRenderer
{
public:
  PlayerRenderer(const PlayerSystem*);
  virtual void draw() const override;
private:
  const PlayerSystem* playerSystem;
};
