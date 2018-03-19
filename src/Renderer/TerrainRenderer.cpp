#include "TerrainRenderer.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Terrain.hpp"
#include "../ResourceManager.hpp"

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

  const std::map<float, float>& points = terrain->points;
  verts.clear();

  for (auto i = points.begin(); i != points.end(); ++i) {
    const auto& p1 = *i;
    
    if (i != std::prev(points.end())) {
      const auto& p2 = *(std::next(i));
      verts.push_back({p1.first, p1.second});
      verts.push_back({p1.first, 0.f});
      verts.push_back({p2.first, p2.second});
    }

    if (i != points.begin()) {
      const auto& p2 = *(std::prev(i));
      verts.push_back({p1.first, p1.second});
      verts.push_back({p2.first, 0.f});
      verts.push_back({p1.first, 0.f});
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2),
      &verts[0], GL_STATIC_DRAW);

  shader.setMat4("model", glm::mat4());

  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, verts.size());
  glBindVertexArray(0);
}
