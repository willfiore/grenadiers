#pragma once

#include <boost/variant.hpp>
#include <vector>

#include <glm/vec2.hpp>

struct Event
{
  enum Type {
    KEY_PRESS,
    EXPLOSION
  };

  Event(Type t) : type(t) {}

  Type type;

  std::vector<boost::variant<
    glm::vec2,
    float,
    int,
    bool

    >> data;
};
