#include "ProjectileRenderer.hpp"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../ProjectileSystem.hpp"

ProjectileRenderer::ProjectileRenderer(const ProjectileSystem* p) :
  projectileSystem(p)
{
  std::vector<glm::vec2> verts;

  verts.push_back({-1.f, +0.f});
  verts.push_back({+1.f, +2.f});
  verts.push_back({-1.f, +2.f});

  verts.push_back({-1.f, +0.f});
  verts.push_back({+1.f, +0.f});
  verts.push_back({+1.f, +2.f});

  GLuint VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2),
      &verts[0], GL_STATIC_DRAW);

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("base");
}

void ProjectileRenderer::draw() const
{
  glBindVertexArray(VAO);
  shader.use();

  for (auto g : projectileSystem->getGrenades()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(g.position, 1.0));
    model = glm::scale(model, glm::vec3(Grenade::SIZE, Grenade::SIZE, 1.f));

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindVertexArray(0);
}
