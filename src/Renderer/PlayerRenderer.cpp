#include "PlayerRenderer.hpp"

#include <vector>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../ResourceManager.hpp"
#include "../Player.hpp"

PlayerRenderer::PlayerRenderer(const PlayerSystem* p) :
  playerSystem(p)
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

void PlayerRenderer::draw() const
{
  glBindVertexArray(VAO);
  shader.use();
  
  for (auto p : playerSystem->getPlayers()) {
    glm::mat4 model = glm::mat4();
    model = glm::translate(model, glm::vec3(p.position, 1.0));
    model = glm::rotate(model, p.angle, glm::vec3(0.f, 0.f, 1.f));
    model = glm::scale(model, glm::vec3(Player::SIZE, Player::SIZE, 1.f));

    shader.setMat4("model", model);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glBindVertexArray(0);
}
