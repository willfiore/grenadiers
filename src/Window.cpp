#include "Window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "EventManager.hpp"
#include "ResourceManager.hpp"

void glfw_key_callback(GLFWwindow* window,
    int key, int scancode, int action, int mods)
{
}

Window::Window() :
  width(1366),
  height(768),
  fullscreen(true),
  multisamples(16)
{
  name = "Platformer";

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(
      width, height,
      name.c_str(),
      fullscreen ? glfwGetPrimaryMonitor() : NULL,
      NULL
      );

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSwapInterval(1.0);

  // Init GLAD
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // Callbacks
  glfwSetKeyCallback(window, glfw_key_callback);

  // Generate pre-post MSAA FBO
  glGenFramebuffers(1, &FBO);
  glGenTextures(1, &FBO_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, FBO_buffer);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisamples,
      GL_RGB, width, height, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D_MULTISAMPLE, FBO_buffer, 0);
  // Generate depth buffer
  unsigned int FBO_RBO;
  glGenRenderbuffers(1, &FBO_RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, FBO_RBO);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisamples,
      GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO_RBO);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Post processing downsampled framebuffers
  glGenFramebuffers(2, PFBO);
  glGenTextures(2, PFBO_buffer);
  for (int i = 0; i < 2; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, PFBO[i]);
    glBindTexture(GL_TEXTURE_2D, PFBO_buffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
	GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
	GL_TEXTURE_2D, PFBO_buffer[i], 0);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  float screenQuadVerts[] = { 
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
    1.0f,  1.0f,  1.0f, 1.0f
  };

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts),
      &screenQuadVerts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
      (void*)(2 * sizeof(float)));
}

void Window::initShaders()
{
  // Set up shaders
  shader_post = ResourceManager::GetShader("post");
  shader_post.use();
  shader_post.setInt("screenTexture", 0);
}

void Window::render()
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, PFBO[0]);

  glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
      GL_COLOR_BUFFER_BIT, GL_NEAREST);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDisable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT);

  shader_post.use();

  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, PFBO_buffer[0]);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

