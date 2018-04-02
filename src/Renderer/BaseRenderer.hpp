#pragma once

#include <glad/glad.h>
#include "../Shader.hpp"

class BaseRenderer
{
public:
  BaseRenderer();
  virtual void draw() const = 0;

  static void InitSharedVertexData();
protected:
  const static GLint VERTS_BEGIN_QUAD = 0;
  const static GLint VERTS_BEGIN_LINE = 6;

  static GLuint sharedVAO;
  static GLuint sharedVBO;
};
