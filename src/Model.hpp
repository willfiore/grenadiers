#pragma once

#include <vector>
#include <glm/vec3.hpp>

class Model
{
public:
  Model() {};
  Model(
      const std::vector<glm::vec3>& v,
      const std::vector<unsigned int>& i);

  std::vector<glm::vec3> vertices;
  std::vector<unsigned int> indices;

  void setVertices(std::vector<glm::vec3>&&);
  void setIndices (std::vector<unsigned int>&& i);

  void draw() const;
  void drawWireframe() const;

private:
  unsigned int VAO, VBO, EBO;
};
