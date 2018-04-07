#pragma once

#include <vector>
#include "BaseRenderer.hpp"

class Terrain;

class TerrainRenderer : public BaseRenderer
{
public:
  TerrainRenderer(const Terrain&);
  virtual void draw() override;
private:
  const Terrain& terrain;
  float depth;

  GLuint VAO;
  GLuint VBO;
  GLuint EBO;

  std::vector<glm::vec3> verts;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  Shader shader;
};
