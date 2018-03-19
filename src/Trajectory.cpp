#include "Trajectory.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "ResourceManager.hpp"

Trajectory::Trajectory()
{
  shader = ResourceManager::GetShader("base");

  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
      2 * sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Trajectory::update(glm::vec2 p, glm::vec2 v)
{
  points.clear();
  model = glm::translate(glm::mat4(),
      glm::vec3(p, 0.f));

  float gravity = -30.f;

  for (float t = 0; t < 12; t += 0.1) {
    float x = v.x*t;
    float y = v.y*t + 0.5*gravity*t*t;

    points.push_back({x, y});
  }
}

void Trajectory::draw() const
{
  shader.setMat4("model", model);
  shader.use();

  glBindVertexArray(VAO);

  std::vector<glm::vec2> verts;

  float s = 3;
  for (const auto& i : points) {
    verts.push_back(i);
    // verts.push_back({i.x - s, i.y - s});
    // verts.push_back({i.x + s, i.y - s});

    // verts.push_back({i.x + s, i.y - s});
    // verts.push_back({i.x + s, i.y + s});

    // verts.push_back({i.x + s, i.y + s});
    // verts.push_back({i.x - s, i.y + s});

    // verts.push_back({i.x - s, i.y + s});
    // verts.push_back({i.x - s, i.y - s});
  }

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec2),
      &verts[0], GL_STREAM_DRAW);

  glDrawArrays(GL_LINE_STRIP, 0, verts.size());
  glBindVertexArray(0);
}

