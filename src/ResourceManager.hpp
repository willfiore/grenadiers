#pragma once

#include <string>
#include <map>

#include "Shader.hpp"

class ResourceManager {
public:
  static constexpr char SHADER_PATH[] = "../assets/shaders/";

  static Shader LoadShader(std::string name, const GLchar* vertexShaderFile, const GLchar* fragmentShaderFile);
  static Shader GetShader(std::string name);
private:
  ResourceManager() {};

  static std::map<std::string, Shader> shaders;
};
