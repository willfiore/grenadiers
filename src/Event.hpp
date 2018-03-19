#pragma once

#include <glm/vec2.hpp>

struct Event
{
  enum Type {
    EXPLOSION
  };

  Event(Type t) : type(t) {}

  Type type;
  glm::vec2 position;
  float radius;
};
