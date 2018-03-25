#pragma once

#include <boost/any.hpp>
#include <vector>

#include <glm/vec2.hpp>

struct Event
{
  enum Type {
    KEY_PRESS,
    PLAYER_FIRE_WEAPON,
    EXPLOSION
  };

  Event(Type t);

  Type type;

  float timestamp;
  std::vector<boost::any> data;
};
