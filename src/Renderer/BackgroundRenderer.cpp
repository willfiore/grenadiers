#include "BackgroundRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../ResourceManager.hpp"
#include "../Random.hpp"
#include "../Terrain.hpp"

#include "../Console.hpp"

BackgroundRenderer::BackgroundRenderer(const Terrain* t) :
  depth(100),
  terrain(t)
{

  glGenBuffers(1, &VBO);
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT),
      (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  shader = ResourceManager::GetShader("bg_mesh");
}

void BackgroundRenderer::draw()
{
  const std::vector<glm::vec2>& terrainPoints = terrain->getPoints();
  int width = terrainPoints.size();

  std::vector<glm::vec3> points;

  for (int d = 0; d < depth; ++d) {
    for (const auto& p : terrainPoints) {
      glm::vec3 point {p.x, 0.f, -100.f-d*Terrain::PRECISION};

      if (d == 0) {
	point.y = p.y;
      }
      else {
	float modifier = d*0.06f;
	if (modifier > 1.f) modifier = 1.f;
	point.y = p.y + modifier * (-200.f - p.y);
	point.y += 20.f*Random::randomFloat();
      }
      
      points.push_back(point);
    }
  }

  std::vector<glm::vec3> verts;

  // Generate verts
  for (int d = 0; d < depth; ++d) {
    for (int i = 0; i < width; ++i) {
      if (i != width-1 && d != depth-1) {
	glm::vec3 p1 = points[d*width+i];
	glm::vec3 p2 = points[d*width+i+1];
	glm::vec3 p3 = points[(d+1)*width+i+1];
	glm::vec3 normal = glm::normalize(glm::cross(p1-p2, p3-p1));
	verts.push_back(p1);
	verts.push_back(normal);
	verts.push_back(p2);
	verts.push_back(normal);
	verts.push_back(p3);
	verts.push_back(normal);
      }
      if (i != width-1 && d != 0) {
	glm::vec3 p1 = points[d*width+i];
	glm::vec3 p2 = points[(d-1)*width+i];
	glm::vec3 p3 = points[d*width+i+1];
	glm::vec3 normal = glm::normalize(glm::cross(p1-p2, p3-p1));
	verts.push_back(p1);
	verts.push_back(normal);
	verts.push_back(p2);
	verts.push_back(normal);
	verts.push_back(p3);
	verts.push_back(normal);
      }
    }
  }
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glm::mat4 model;
  shader.use();
  shader.setMat4("model", model);

  glDrawArrays(GL_TRIANGLES, 0, verts.size());
  glBindVertexArray(0);
}
