#pragma once

#include <string>

#include "Event.hpp"

class GLFWwindow;

class Window
{
public:
  Window();

  int getWidth() const { return width; }
  int getHeight() const { return height; }
  int isFullscreen() const { return fullscreen; }

  int getFBO() const { return FBO; }

  void render();

  const GLFWwindow* getWindow() const { return window; }
  GLFWwindow* getWindow() { return window; }

private:
  GLFWwindow* window;
  // Window props
  int width;
  int height;
  bool fullscreen;
  int multisamples;
  std::string name;

  // Render buffers
  unsigned int FBO, FBO_buffer; // Pre-post multisample framebuffer
  unsigned int PFBO, PFBO_buffer; // Post processing downsampled buffer
  unsigned int VAO, VBO; // final quad to draw

  void generateFBO();
};
