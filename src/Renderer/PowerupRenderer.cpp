#include "PowerupRenderer.hpp"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../PowerupSystem.hpp"

PowerupRenderer::PowerupRenderer(const PowerupSystem* p) :
  powerupSystem(p)
{
  std::vector<glm::vec3> verts;

  verts.push_back({-1.f, +0.f, 0.f});
  verts.push_back({+1.f, +2.f, 0.f});
  verts.push_back({-1.f, +2.f, 0.f});

  verts.push_back({-1.f, +0.f, 0.f});
  verts.push_back({+1.f, +0.f, 0.f});
  verts.push_back({+1.f, +2.f, 0.f});

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("base");
}

void PowerupRenderer::draw() const
{
  glBindVertexArray(VAO);
  shader.use();

  for (auto p : powerupSystem->getPowerups()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(p.position, 1.0));
    model = glm::scale(model, glm::vec3(6.f, 6.f, 1.f));

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindVertexArray(0);
}
