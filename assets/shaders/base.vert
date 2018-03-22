#version 330 core
layout (std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

layout (location = 0) in vec3 pos;

void main() {
  gl_Position = projection * view * model * vec4(pos, 1.0);
}
