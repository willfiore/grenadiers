#version 330 core
layout (std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDir = vec3(0.0, -1.0, 0.0);
  vec3 lightColor = 1.0*vec3(1.0, 0.0, 0.0);

  vec3 diffuse = vec3(max(dot(norm, lightDir), 0.0)) * lightColor;
  FragColor = vec4(diffuse, 1.0);
}
