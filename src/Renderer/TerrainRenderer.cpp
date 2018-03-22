#include "TerrainRenderer.hpp"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "../Terrain.hpp"
#include "../ResourceManager.hpp"

#include <iostream>

TerrainRenderer::TerrainRenderer(const Terrain *t) :
  terrain(t),
  depth(100000.f)
{
  glGenBuffers(1, &VBO);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
      3 * sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("terrain");
}

void TerrainRenderer::draw() const
{
  glBindVertexArray(VAO);
  shader.use();

  const std::vector<glm::vec2>& points = terrain->getPoints();
  std::vector<glm::vec3> verts;

  float forwardDepth = 40.f;
  float backwardDepth = -10.f;
  
  for (size_t i = 0; i < points.size(); ++i) {
    const glm::vec2& p1 = points[i];

    if (i != points.size() - 1) {
      const glm::vec2& p2 = points[i+1];

      // 2D front
      verts.push_back({p1.x, p1.y, forwardDepth});
      verts.push_back({p1.x, -depth, forwardDepth});
      verts.push_back({p2.x, p2.y, forwardDepth});

      // 3D
      verts.push_back({p1.x, p1.y, forwardDepth});
      verts.push_back({p2.x, p2.y, forwardDepth});
      verts.push_back({p1.x, p1.y, backwardDepth});
    }

    if (i != 0) {
      const glm::vec2& p2 = points[i-1];

      verts.push_back({p1.x, p1.y, forwardDepth});
      verts.push_back({p2.x, -depth, forwardDepth});
      verts.push_back({p1.x, -depth, forwardDepth});
      
      // 3D
      verts.push_back({p1.x, p1.y, forwardDepth});
      verts.push_back({p1.x, p1.y, backwardDepth});
      verts.push_back({p2.x, p2.y, backwardDepth});
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  shader.setMat4("model", glm::mat4());

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_TRIANGLES, 0, verts.size());
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(0);
}
