#pragma once

#include <vector>
#include "BaseRenderer.hpp"

class Terrain;

class BackgroundRenderer : public BaseRenderer
{
public:
  BackgroundRenderer(const Terrain*);
  virtual void draw() override;
private:
  GLuint VAO;
  GLuint VBO;

  int depth;

  Shader shader;
  const Terrain* terrain;
};
