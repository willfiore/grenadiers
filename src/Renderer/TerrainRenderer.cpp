#include "TerrainRenderer.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "../Terrain.hpp"
#include "../ResourceManager.hpp"

#include <iostream>

TerrainRenderer::TerrainRenderer(const Terrain *t) :
  terrain(t)
{
  glGenBuffers(1, &VBO);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
      2 * sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("base");
}

void TerrainRenderer::draw() const
{
  const std::vector<glm::vec2>& points = terrain->getPoints();
  std::vector<glm::vec2> verts;

  for (size_t i = 0; i < points.size(); ++i) {
    const glm::vec2& p1 = points[i];

    if (i != points.size() - 1) {
      const glm::vec2& p2 = points[i+1];

      verts.push_back({p1.x, p1.y});
      verts.push_back({p1.x, 0.f});
      verts.push_back({p2.x, p2.y});
    }

    if (i != 0) {
      const glm::vec2& p2 = points[i-1];

      verts.push_back({p1.x, p1.y});
      verts.push_back({p2.x, 0.f});
      verts.push_back({p1.x, 0.f});
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2),
      &verts[0], GL_STREAM_DRAW);

  shader.setMat4("model", glm::mat4());

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, verts.size());
  glBindVertexArray(0);
}
