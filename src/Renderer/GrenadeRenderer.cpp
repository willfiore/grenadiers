#include "GrenadeRenderer.hpp"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "../ResourceManager.hpp"
#include "../GrenadeSystem.hpp"

GrenadeRenderer::GrenadeRenderer(const GrenadeSystem& p) :
  grenadeSystem(p)
{
  shader = ResourceManager::GetShader("base");
  grenadeModel = ResourceManager::GetModel("quad");
}

void GrenadeRenderer::draw()
{
  shader.use();

  for (auto& p : grenadeSystem.getGrenades()) {
    if (p.dirty_awaitingRemoval) continue;

    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(p.position, 0.f));
    model = glm::scale(model, glm::vec3(3.f, 3.f, 1.f));

    shader.setMat4("model", model);

    grenadeModel->draw();
  }
}
