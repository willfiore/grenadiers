#include "BaseRenderer.hpp"

#include <vector>
#include <glad/glad.h>

// Defaults for shared VAO and VBO get overwritten later
GLuint BaseRenderer::sharedVAO = 0;
GLuint BaseRenderer::sharedVBO = 0;

BaseRenderer::BaseRenderer()
{
}

void BaseRenderer::InitSharedVertexData()
{
  std::vector<glm::vec3> verts;

  // Quad
  verts.push_back({-1.f, -1.f, 0.f});
  verts.push_back({1.f, -1.f, 0.f});
  verts.push_back({1.f, 1.f, 0.f});
  verts.push_back({-1.f, -1.f, 0.f});
  verts.push_back({1.f, 1.f, 0.f});
  verts.push_back({-1.f, 1.f, 0.f});
  // Line
  verts.push_back({0.f, 0.f, 0.f});
  verts.push_back({1.f, 0.f, 0.f});

  glGenBuffers(1, &sharedVBO);
  glBindBuffer(GL_ARRAY_BUFFER, sharedVBO);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3),
      &verts[0], GL_STATIC_DRAW);

  glGenVertexArrays(1, &sharedVAO);
  glBindVertexArray(sharedVAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GL_FLOAT), (void*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}
