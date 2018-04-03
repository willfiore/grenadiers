#include "BeamRenderer.hpp"

#include <vector>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../PlayerSystem.hpp"

BeamRenderer::BeamRenderer(const PlayerSystem* p) :
  playerSystem(p)
{
  shader = ResourceManager::GetShader("base");
}

void BeamRenderer::draw() const
{
  glBindVertexArray(sharedVAO);
  shader.use();

  for (const auto& p : playerSystem->getPlayers()) {
    if (!p.firingBeam) continue;

    glm::mat4 model = glm::mat4();

    model = glm::translate(model,
	glm::vec3(p.getCenterPosition(), 0.f));
    model = glm::rotate(model, -p.aimDirection, glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, glm::vec3({10000.f, 2.f, 1.f}));
    model = glm::translate(model, glm::vec3({1.f, 0.f, 0.f}));

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, VERTS_BEGIN_QUAD, 6);
  }

  glBindVertexArray(0);
}
