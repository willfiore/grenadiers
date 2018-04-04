#include "TimescaleSystem.hpp"

#include "EventManager.hpp"

TimescaleSystem::TimescaleSystem()
{
  timescale = 1.f;
}

void TimescaleSystem::update(double t, double dt)
{
  // Note: t and dt realtime, not simtime, unlike
  // the update functions of other systems

  // Min timescale
  if (timescale < 0.01f) timescale = 0.01f;
}
