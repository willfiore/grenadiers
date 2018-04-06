#pragma once

#include "BaseRenderer.hpp"

class PlayerSystem;

class BeamRenderer : public BaseRenderer {
public:
  BeamRenderer(const PlayerSystem*);
  virtual void draw() override;

private:
  const PlayerSystem* playerSystem;
  Shader shader;
};
