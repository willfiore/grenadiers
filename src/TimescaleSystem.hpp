#pragma once

class TimescaleSystem
{
public:
  TimescaleSystem();

  void update(float t, float dt);
  float getTimescale() const { return timescale; }

private:
  float timescale;
};
