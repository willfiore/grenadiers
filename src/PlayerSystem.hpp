#pragma once

#include <vector>

#include "Player.hpp"
#include "EventManager.hpp"

class Terrain;
class ProjectileSystem;

class PlayerSystem
{
public:
  PlayerSystem(const Terrain*, ProjectileSystem*);

  void update(float dt, const float* axes);
  void processInput(int playerID, int button, bool action);

  void jump(Player&);
  void launchGrenade(Player&);

  const std::vector<Player>& getPlayers() const { return players; };

  void onExplosion(Event);

private:
  std::vector<Player> players;
  const Terrain* terrain;
  ProjectileSystem* projectileSystem;
};
