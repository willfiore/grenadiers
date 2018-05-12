#pragma once

#include <vector>
#include <map>
#include "ControllerData.hpp"

#include "Player.hpp"

class Terrain;
class ProjectileSystem;
class TimescaleSystem;
struct Event;

class PlayerSystem
{
public:
  PlayerSystem(
      const Terrain&,
      const std::map<int, ControllerData>&,
      const TimescaleSystem&
      );

  void update(double t, double dt);
  void processInput(int controllerID, int button, bool action);

  void jump(Player&);
  void throwGrenade(Player&);
  void detonateGrenade(Player&);
  void cycleGrenade(Player&);
  void kill(Player&);
  void respawn(Player&);

  const std::vector<Player>& getPlayers() const { return players; };
  const Player& getPlayer(int id) const;

private:
  std::vector<Player> players;

  const Terrain& terrain;
  const std::map<int, ControllerData>& controllers;
  const TimescaleSystem& timescaleSystem;

  void onExplosion(const Event&);
  void onPowerupPickup(const Event&);
};
