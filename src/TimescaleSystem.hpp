#pragma once

#include <glm/vec2.hpp>
#include <vector>

struct Event;

class TimescaleSystem
{
public:
  struct Zone {
    glm::vec2 position;
    float radius;
    double timescale;
  };

  TimescaleSystem();

  void update(double t, double dt);
  double getGlobalTimescale() const { return globalTimescale; }
  double getTimescaleAtPosition (glm::vec2) const;
  const std::vector<Zone>& getZones() const { return zones; }

private:
  double inertiaFactor;
  double globalTimescale;

  std::vector<Zone> zones;
  Zone& addZone();

  void onExplosion(const Event&);
};
