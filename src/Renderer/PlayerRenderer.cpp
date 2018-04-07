#include "PlayerRenderer.hpp"

#include <vector>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../Player.hpp"
#include "../PlayerSystem.hpp"

PlayerRenderer::PlayerRenderer(const PlayerSystem& p) :
  playerSystem(p)
{
  shader = ResourceManager::GetShader("base");
}

void PlayerRenderer::draw()
{
  shader.use();
  
  for (const auto p : playerSystem.getPlayers()) {
    glm::mat4 model = glm::mat4();
    // Move to player position
    model = glm::translate(model, glm::vec3(p.position, 0.f));
    // Rotate to player angle
    model = glm::rotate(model, p.angle, glm::vec3(0.f, 0.f, 1.f));
    // Size to player
    model = glm::scale(model, glm::vec3(Player::SIZE, Player::SIZE, 1.f));
    // Move origin to bottom middle
    model = glm::translate(model, glm::vec3({0.f, 1.f, 0.f}));
    shader.setMat4("model", model);

    drawPrimitive(Primitive::QUAD);

    // Draw aim direction
    model = glm::mat4();
    model = glm::translate(model, {p.getCenterPosition(), 0.f});
    model = glm::rotate(model, -p.aimDirection, {0.f, 0.f, 1.f});
    model = glm::scale(model, {2*Player::SIZE, 1.f, 1.f});
    shader.setMat4("model", model);
    drawPrimitive(Primitive::LINE);
  }
}
