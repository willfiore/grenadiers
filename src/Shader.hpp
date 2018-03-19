#pragma once

#include <glad/glad.h>
#include <string>
#include <glm/mat4x4.hpp>

class Shader
{
public:
  unsigned int ID;

  Shader();
  void compile(const char* vertexCode, const char* fragCode);
  void use() const;

  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMat4(const std::string &name, glm::mat4 value) const;
};
