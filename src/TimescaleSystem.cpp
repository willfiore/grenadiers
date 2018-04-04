#include "TimescaleSystem.hpp"

#include "EventManager.hpp"

TimescaleSystem::TimescaleSystem()
{
  timescale = 0.1f;
}

void TimescaleSystem::update(double t, double dt)
{
  // Note: t and dt realtime, not simtime, unlike
  // the update functions of other modules

  // Min timescale
  if (timescale < 0.01f) timescale = 0.01f;
}
