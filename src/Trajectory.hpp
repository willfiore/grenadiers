#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <glm/matrix.hpp>
#include "Shader.hpp"

class Trajectory {
public:
  Trajectory();
  void draw() const;

  void update(glm::vec2 p, glm::vec2 v);

private:
  // Drawing
  unsigned int VAO;
  unsigned int VBO;
  glm::mat4 model;
  std::vector<glm::vec2> points;
  std::vector<glm::vec2> verts;
  Shader shader;
};

