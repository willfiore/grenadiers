#include "ResourceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Initialize statics
std::map<std::string, Shader> ResourceManager::shaders;

Shader ResourceManager::LoadShader(std::string name,
    const GLchar* vertexRelativePath,
    const GLchar* fragmentRelativePath)
{
  std::string vertexPath = SHADER_PATH;
  std::string fragmentPath = SHADER_PATH;
  vertexPath.append(vertexRelativePath);
  fragmentPath.append(fragmentRelativePath);

  std::string vertexCode, fragmentCode;
  std::ifstream vertexFile, fragmentFile;

  vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  // 1) Read shaders from file
  try {
    // Open files
    vertexFile.open(vertexPath);
    fragmentFile.open(fragmentPath);

    // Read into string streams
    std::stringstream vertexStream, fragmentStream;
    vertexStream << vertexFile.rdbuf();
    fragmentStream << fragmentFile.rdbuf();

    // Close files
    vertexFile.close();
    fragmentFile.close();

    // convert streams into strings
    vertexCode = vertexStream.str();
    fragmentCode = fragmentStream.str();
  } 
  catch(std::ifstream::failure e) {
    std::cout << e.what() << std::endl;
  }

  const char* vertexCode_cstr = vertexCode.c_str();
  const char* fragmentCode_cstr = fragmentCode.c_str();

  Shader shader;
  shader.compile(vertexCode_cstr, fragmentCode_cstr);

  // Set UBO bindings
  unsigned int uboIndex = glGetUniformBlockIndex(shader.ID, "Matrices");
  glUniformBlockBinding(shader.ID, uboIndex, 0);

  shaders[name] = shader;
  return shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
  return shaders[name];
}
