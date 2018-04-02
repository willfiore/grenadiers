#include "ProjectileRenderer.hpp"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../ResourceManager.hpp"
#include "../ProjectileSystem.hpp"

ProjectileRenderer::ProjectileRenderer(const ProjectileSystem* p) :
  projectileSystem(p)
{
  shader = ResourceManager::GetShader("base");
}

void ProjectileRenderer::draw() const
{
  glBindVertexArray(sharedVAO);
  shader.use();

  for (auto& p : projectileSystem->getProjectiles()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(p.position, 0.f));
    model = glm::scale(model, glm::vec3(3.f, 3.f, 1.f));

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, VERTS_BEGIN_QUAD, 6);
  }

  glBindVertexArray(0);
}
