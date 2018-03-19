#pragma once

#include <glad/glad.h>
#include "../Shader.hpp"

class BaseRenderer
{
public:
  BaseRenderer();
  virtual void draw() const = 0;

protected:
  GLuint VAO;
  Shader shader;
};
