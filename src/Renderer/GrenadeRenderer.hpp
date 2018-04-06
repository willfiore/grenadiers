#pragma once

#include "BaseRenderer.hpp"

class GrenadeSystem;

class GrenadeRenderer : public BaseRenderer {
public:
  GrenadeRenderer(const GrenadeSystem*);
  virtual void draw() override;

private:
  const GrenadeSystem* grenadeSystem;
  Shader shader;
};
