#include "TimescaleZoneRenderer.hpp"

#include "../ResourceManager.hpp"
#include "../TimescaleSystem.hpp"

#include <glm/gtc/matrix_transform.hpp>

TimescaleZoneRenderer::TimescaleZoneRenderer(const TimescaleSystem& t) :
  timescaleSystem(t)
{
  shader = ResourceManager::GetShader("inertia_zone");
}

void TimescaleZoneRenderer::draw()
{
  shader.use();

  for (auto& z : timescaleSystem.getZones()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(z.position, 0.f));
    model = glm::scale(model, {z.radius, z.radius, 1.f});

    shader.setMat4("model", model);
    drawPrimitive(Primitive::CIRCLE);
  }
}
