#include "PowerupRenderer.hpp"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../PowerupSystem.hpp"

PowerupRenderer::PowerupRenderer(const PowerupSystem& p) :
  powerupSystem(p)
{
  shader = ResourceManager::GetShader("base");
  powerupModel = ResourceManager::GetModel("quad");
}

void PowerupRenderer::draw()
{
  shader.use();

  for (auto p : powerupSystem.getPowerups()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(p.position, 0.f));
    model = glm::scale(model, glm::vec3(6.f, 6.f, 1.f));
    model = glm::translate(model, glm::vec3({0.f, 1.f, 0.f}));

    shader.setMat4("model", model);
    powerupModel->draw();
  }
}
