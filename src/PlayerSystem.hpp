#pragma once

#include <vector>
#include "ControllerData.hpp"

#include "Player.hpp"
#include "EventManager.hpp"

class Terrain;
class ProjectileSystem;

class PlayerSystem
{
public:
  PlayerSystem(const Terrain*, const std::map<int, ControllerData>*);

  void update(float t, float dt);
  void processInput(int controllerID, int button, bool action);

  void jump(Player&);
  void fireWeapon(Player&);
  void releaseWeapon(Player&);
  void secondaryFireWeapon(Player&);
  void cycleWeapon(Player&);

  const std::vector<Player>& getPlayers() const { return players; };

private:
  std::vector<Player> players;

  const Terrain* terrain;
  const std::map<int, ControllerData>* controllers;

  void onExplosion(Event);
  void onPowerupPickup(Event);
};
