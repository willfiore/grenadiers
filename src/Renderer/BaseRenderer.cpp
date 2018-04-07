#include "BaseRenderer.hpp"

#include <vector>
#include <glad/glad.h>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>

// Defaults for shared VAO and VBO get overwritten later
GLuint BaseRenderer::sharedVAO = 0;
GLuint BaseRenderer::sharedVBO = 0;
GLuint BaseRenderer::sharedEBO = 0;

BaseRenderer::BaseRenderer()
{
}

void BaseRenderer::InitSharedVertexData()
{
  std::vector<glm::vec3> verts;
  std::vector<unsigned int> indices;

  // 0 point
  verts.push_back({0.f, 0.f, 0.f});
  // Quad
  verts.push_back({-1.f, -1.f, 0.f});
  verts.push_back({1.f, -1.f, 0.f});
  verts.push_back({1.f, 1.f, 0.f});
  verts.push_back({-1.f, 1.f, 0.f});
  indices.insert(indices.end(), {1, 2, 3, 1, 3, 4});
  // Line
  verts.push_back({1.f, 0.f, 0.f});
  indices.insert(indices.end(), {0, 5});
  // Circle
  unsigned int circleNumPoints = 100;
  float divisionSize = glm::two_pi<float>() / circleNumPoints;
  for (unsigned int i = 0; i <= circleNumPoints; ++i) {
    unsigned int j = i;
    if (i == circleNumPoints) j = 0;
    float angle = i*divisionSize;
    verts.push_back({glm::cos(angle), glm::sin(angle), 0.f});
    indices.insert(indices.end(), {0, 6+i, 7+j});
  }

  glGenVertexArrays(1, &sharedVAO);
  glBindVertexArray(sharedVAO);

  glGenBuffers(1, &sharedVBO);
  glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STATIC_DRAW);

  glGenBuffers(1, &sharedEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
      &indices[0], GL_STATIC_DRAW);


  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void BaseRenderer::drawPrimitive(Primitive p)
{
  glBindVertexArray(sharedVAO);

  switch (p) {
    case Primitive::QUAD:
      glDrawElements(GL_TRIANGLES, SIZE_QUAD, GL_UNSIGNED_INT,
	  (void*)(sizeof(unsigned int) * BEGIN_QUAD));
      break;
    case Primitive::LINE:
      glDrawElements(GL_LINES, SIZE_LINE, GL_UNSIGNED_INT,
	  (void*)(sizeof(unsigned int) * BEGIN_LINE));
      break;
    case Primitive::CIRCLE:
      glDrawElements(GL_TRIANGLES, SIZE_CIRCLE, GL_UNSIGNED_INT,
	  (void*)(sizeof(unsigned int) * BEGIN_CIRCLE));
      break;
    default: break;
  }

  glBindVertexArray(0);
}
