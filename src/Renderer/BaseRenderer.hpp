#pragma once

#include <glad/glad.h>
#include "../Shader.hpp"

class BaseRenderer
{
public:
  BaseRenderer();
  virtual void draw() = 0;

  static void InitSharedVertexData();
protected:
  enum class Primitive {
    QUAD,
    LINE,
    CIRCLE
  };

  void drawPrimitive(Primitive);

private:
  const GLuint BEGIN_QUAD = 0;
  const GLuint SIZE_QUAD = 6;
  const GLuint BEGIN_LINE = 6;
  const GLuint SIZE_LINE = 2;
  const GLuint BEGIN_CIRCLE = 8;
  const GLuint SIZE_CIRCLE = 100*3;

  static GLuint sharedVAO;
  static GLuint sharedVBO;
  static GLuint sharedEBO;
};
