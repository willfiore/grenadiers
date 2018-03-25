#version 330 core
layout (std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  FragPos = vec3(model * vec4(aPos, 1.0));
  Normal = aNormal;
}
