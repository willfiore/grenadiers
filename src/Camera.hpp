#pragma once

#include <glm/vec2.hpp>

struct Camera {

  static constexpr float ACCEL = 0.06f;

  glm::vec2 position;
};
