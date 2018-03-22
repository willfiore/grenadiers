#pragma once

#include "BaseRenderer.hpp"

class PowerupSystem;

class PowerupRenderer : public BaseRenderer {
public:
  PowerupRenderer(const PowerupSystem*);
  virtual void draw() const override;

private:
  const PowerupSystem* powerupSystem;
};
