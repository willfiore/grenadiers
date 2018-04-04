#pragma once

#include "Event.hpp"

class TimescaleSystem
{
public:
  TimescaleSystem();

  void update(double t, double dt);
  float getTimescale() const { return timescale; }

private:
  double timescale;
};
