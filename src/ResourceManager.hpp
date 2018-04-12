#pragma once

#include <string>
#include <map>

#include "Shader.hpp"
#include "Model.hpp"

class ResourceManager {
public:
  static constexpr char SHADER_PATH[] = "../assets/shaders/";
  static constexpr char MODEL_PATH[] = "../assets/models/";

  static void LoadShader(const std::string& name,
      const std::string& vertexShaderFile,
      const std::string& fragmentShaderFile);
  static Shader GetShader(std::string name) { return shaders[name]; }

  static void LoadModel(const std::string& name,
      const std::string& filename);

  static void SaveModel(const Model& model, const std::string& filename);
  static const Model* GetModel(std::string name) { return &models[name]; }

private:
  ResourceManager() {};

  static std::map<std::string, Shader> shaders;
  static std::map<std::string, Model> models;
};
