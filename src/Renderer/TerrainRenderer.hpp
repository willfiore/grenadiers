#pragma once

#include <vector>
#include "BaseRenderer.hpp"

class Terrain;

class TerrainRenderer : public BaseRenderer
{
public:
  TerrainRenderer(const Terrain*);
  virtual void draw() const override;
private:
  const Terrain* terrain;

  GLuint VBO;
  mutable std::vector<glm::vec2> verts;
};
