#pragma once

#include <string>

#include "Event.hpp"
#include "Shader.hpp"

class GLFWwindow;

class Window
{
public:
  Window();

  int getWidth() const { return width; }
  int getHeight() const { return height; }

  int getFBO() const { return FBO; }

  void initShaders();
  void render();

  const GLFWwindow* getWindow() const { return window; }
  GLFWwindow* getWindow() { return window; }

private:
  GLFWwindow* window;
  // Window props
  int width;
  int height;
  int multisamples;
  std::string name;

  // Render buffers
  unsigned int FBO, FBO_buffer; // Pre-post multisample framebuffer
  unsigned int PFBO[2], PFBO_buffer[2]; // Post processing downsampled buffer
  unsigned int VAO, VBO; // final quad to draw

  Shader shader_post;
};
