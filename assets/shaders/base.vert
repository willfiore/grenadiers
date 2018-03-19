#version 330 core
layout (std140) uniform Matrices
{
  mat4 projection;
  mat4 view;
};
uniform mat4 model;

layout (location = 0) in vec2 pos;

void main() {
  gl_Position = projection * view * model * vec4(pos.x, pos.y, 0.0, 1.0);
}
