#include "Shader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
{
}

void Shader::compile(const char* vertexCode, const char* fragmentCode)
{
  unsigned int vertex, fragment;
  int success;
  char infoLog[512];

  // Vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexCode, NULL);
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "Failed vertex shader compilation: " << infoLog << std::endl;
  }

  // Fragment shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentCode, NULL);
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "Failed frag shader compilation: " << infoLog << std::endl;
  }

  // 3) Create shader program
  ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);
  glLinkProgram(ID);
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    std::cout << "Failed shader linking: " << infoLog << std::endl;
  }

  // Shaders are now linked into the program,
  // so are no longer needed.
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() const
{
  glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{         
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
}
void Shader::setInt(const std::string &name, int value) const
{ 
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
}
void Shader::setFloat(const std::string &name, float value) const
{ 
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
} 
void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),
      1, GL_FALSE, glm::value_ptr(value));
}
