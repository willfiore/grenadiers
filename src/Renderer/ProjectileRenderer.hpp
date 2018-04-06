#pragma once

#include "BaseRenderer.hpp"

class ProjectileSystem;

class ProjectileRenderer : public BaseRenderer {
public:
  ProjectileRenderer(const ProjectileSystem*);
  virtual void draw() override;

private:
  const ProjectileSystem* projectileSystem;
  Shader shader;
};
