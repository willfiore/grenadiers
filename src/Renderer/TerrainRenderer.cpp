#include "TerrainRenderer.hpp"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <GLFW/glfw3.h>

#include "../Terrain.hpp"
#include "../ResourceManager.hpp"
#include "../Random.hpp"

#include <iostream>

TerrainRenderer::TerrainRenderer(const Terrain *t) :
  terrain(t),
  depth(1000.f)
{
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
      3 * sizeof(GL_FLOAT), (void*)0);

  glEnableVertexAttribArray(0);

  // Index data
  const auto& points = terrain->getPoints();
  for (size_t i = 0; i < points.size()-1; ++i) {
    indices.push_back(2*i);
    indices.push_back(2*i+1);
    indices.push_back(2*(i+1)+1);

    indices.push_back(2*i);
    indices.push_back(2*(i+1)+1);
    indices.push_back(2*(i+1));
  }

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("terrain");
}

void TerrainRenderer::draw()
{
  glBindVertexArray(VAO);
  shader.use();
  shader.setFloat("time", glfwGetTime());

  const auto& points = terrain->getPoints();

  verts.clear();
  normals.clear();

  // Vertex data
  for (size_t i = 0; i < points.size(); ++i) {
    const glm::vec2& p1 = points[i];
    verts.push_back({p1.x, p1.y, 0.f});
    verts.push_back({p1.x, -1000.f, 0.f}); 
  }

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
      &indices[0], GL_STREAM_DRAW);

  shader.setMat4("model", glm::mat4());

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);
}
