#include "ResourceManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

#include "Console.hpp"

// Initialize statics
std::map<std::string, Shader> ResourceManager::shaders;
std::map<std::string, Model> ResourceManager::models;

void ResourceManager::LoadShader(const std::string& name,
    const std::string& vertexRelativePath,
    const std::string& fragmentRelativePath)
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
}

void ResourceManager::LoadModel(const std::string& name,
    const std::string& filename)
{
  std::string path = MODEL_PATH;
  path.append(filename);

  std::ifstream file;
  file.open(path, std::ios::in | std::ios::binary);

  // Data
  unsigned int numVerts, numTris;
  std::vector<glm::vec3> vertices;
  std::vector<unsigned int> indices;

  file.read(reinterpret_cast<char*>(&numVerts), sizeof(unsigned int));
  file.read(reinterpret_cast<char*>(&numTris), sizeof(unsigned int));

  // Allocate correct size for verts
  vertices.resize(numVerts);
  indices.resize(numTris * 3);

  file.read(reinterpret_cast<char*>(&vertices[0]),
      numVerts * sizeof(glm::vec3));
  file.read(reinterpret_cast<char*>(&indices[0]),
      numTris * 3 * sizeof(unsigned int));
  file.close();

  models.insert(std::make_pair(name, Model(vertices, indices)));
}

void ResourceManager::SaveModel(const Model& model, const std::string& filename)
{
  unsigned int numVerts = model.vertices.size();
  unsigned int numTris  = model.indices.size() / 3;

  std::ofstream file;
  std::string path = MODEL_PATH;
  path.append(filename);

  file.open(path, std::ios::out | std::ios::binary);

  // .model file format:
  // [1 x unsigned int] numVerts
  // [1 x unsigned int] numTris
  // [3*numVerts x float] vertices
  // [3*numTris x unsigned int] indices

  file.write(reinterpret_cast<const char*>(&numVerts), sizeof(unsigned int));
  file.write(reinterpret_cast<const char*>(&numTris), sizeof(unsigned int));
  file.write(reinterpret_cast<const char*>(&model.vertices[0]),
      model.vertices.size() * sizeof(glm::vec3));
  file.write(reinterpret_cast<const char*>(&model.indices[0]),
      model.indices.size() * sizeof(unsigned int));

  file.close();
};
