#include "Model.hpp"
#include <glad/glad.h>
#include "Console.hpp"

Model::Model(
    const std::vector<glm::vec3>& v,
    const std::vector<unsigned int>& i) :
  vertices(v),
  indices(i)
{
  // OpenGL object setup
  // For now, 1 VBO per Model - optimise later if necessary
  // ------------------------------------------------------
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
      &vertices[0], GL_STATIC_DRAW);

  // EBO
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
      &indices[0], GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);
};

void Model::draw() const
{
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Model::drawWireframe() const
{
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
