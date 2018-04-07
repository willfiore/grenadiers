#pragma once

#include "BaseRenderer.hpp"

class PowerupSystem;

class PowerupRenderer : public BaseRenderer {
public:
  PowerupRenderer(const PowerupSystem&);
  virtual void draw() override;

private:
  const PowerupSystem& powerupSystem;
  Shader shader;
};
