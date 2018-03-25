#pragma once

#include <boost/any.hpp>
#include <vector>

#include <glm/vec2.hpp>

struct Event
{
  enum Type {
    KEY_PRESS,
    PLAYER_FIRE_WEAPON,
    POWERUP_PICKUP,
    EXPLOSION
  };

  Event(Type t);

  inline Event& operator<< (boost::any a) {
    data.push_back(a);
    return *this;
  }

  inline boost::any operator[] (size_t i) {
    return data[i];
  }

  Type type;

  float timestamp;
  
private:
  std::vector<boost::any> data;
};
