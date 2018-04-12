#pragma once

#include <glad/glad.h>
#include "../Shader.hpp"
#include "../Model.hpp"

class BaseRenderer
{
public:
  BaseRenderer();
  virtual void draw() = 0;
protected:

private:
};
