#pragma once

#include <string>
#include <map>

#include "Shader.hpp"

class ResourceManager {
public:
  static Shader LoadShader(const GLchar* vertexShaderFile, const GLchar* fragmentShaderFile, std::string name);
  static Shader GetShader(std::string name);
private:
  ResourceManager() {};

  static std::map<std::string, Shader> shaders;
};
