#pragma once

#include "BaseRenderer.hpp"

class TextRenderer : public BaseRenderer
{
public:
  TextRenderer();
  virtual void draw() override;
private:
  Shader shader;
  const Model* model;
};
