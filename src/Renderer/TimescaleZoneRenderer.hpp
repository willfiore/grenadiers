#pragma once

#include "BaseRenderer.hpp"

class TimescaleSystem;

class TimescaleZoneRenderer : public BaseRenderer {
public:
  TimescaleZoneRenderer(const TimescaleSystem&);
  virtual void draw() override;

private:
  const TimescaleSystem& timescaleSystem;
  Shader shader;
};
