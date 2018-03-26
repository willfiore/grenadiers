#include <iostream>
#include <sstream>
#include <set>

#include <glad/glad.h>	  // OpenGL bindings
#include <GLFW/glfw3.h>	  // OpenGL helpers

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Player.hpp"

#include "CameraSystem.hpp"
#include "PlayerSystem.hpp"
#include "ProjectileSystem.hpp"
#include "PowerupSystem.hpp"

#include "Renderer/PlayerRenderer.hpp"
#include "Renderer/ProjectileRenderer.hpp"
#include "Renderer/TerrainRenderer.hpp"
#include "Renderer/PowerupRenderer.hpp"

int main() {

  ////////////////////////
  // Window initialization
  ////////////////////////

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwSwapInterval(1);

  int windowWidth = 960;
  int windowHeight = 540;

  // Init window
  GLFWwindow* window =
    glfwCreateWindow(
	windowWidth, windowHeight,
	"Platformer",
	NULL,
	NULL
	);

  if (window == NULL) {
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  // Init GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    return -1;

  glEnable(GL_DEPTH_TEST);

  ////////////////////////////////////////////////
  // Projection and camera transformation matrices
  ////////////////////////////////////////////////

  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  glm::mat4 projection = glm::ortho(
      0.f, (float)windowWidth,
      0.f, (float)windowHeight,
      -1.f, 1.f
      );

  projection = glm::perspective(
      glm::radians(80.f),
      (float)windowWidth / (float)windowHeight,
      0.01f, 10000.f
      );

  // Initialize space in UBO
  unsigned int UBO;
  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, sizeof(glm::mat4));

  // Push initial matrices to UBO
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
      glm::value_ptr(projection));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
      glm::value_ptr(glm::mat4()));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  
  // FBO for first pass render (before post)
  unsigned FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  unsigned int FBO_texColorBuffer;
  glGenTextures(1, &FBO_texColorBuffer);
  glBindTexture(GL_TEXTURE_2D, FBO_texColorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight,
      0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
      FBO_texColorBuffer, 0);

  unsigned int FBO_RBO;
  glGenRenderbuffers(1, &FBO_RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, FBO_RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
      windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO_RBO);

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

  unsigned int screenVAO, screenVBO;
  glGenVertexArrays(1, &screenVAO);
  glGenBuffers(1, &screenVBO);
  glBindVertexArray(screenVAO);
  glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVerts),
      &screenQuadVerts, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
      (void*)(2 * sizeof(float)));

  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "Error: Incomplete frame buffer" << std::endl;

  ResourceManager::LoadShader(
      "../assets/shaders/base.vert", "../assets/shaders/base.frag",
      "base"
      );

  ResourceManager::LoadShader(
      "../assets/shaders/terrain.vert", "../assets/shaders/terrain.frag",
      "terrain"
      );

  Shader shader_post = ResourceManager::LoadShader(
      "../assets/shaders/post.vert", "../assets/shaders/post.frag",
      "post"
      );

  shader_post.use();
  shader_post.setInt("screenTexture", 0);
  
  Terrain terrain;

  PlayerSystem playerSystem(&terrain);
  ProjectileSystem projectileSystem(&terrain);
  PowerupSystem powerupSystem(&terrain, &playerSystem);
  CameraSystem cameraSystem(&playerSystem);
  cameraSystem.setWindowDimensions(windowWidth, windowHeight);

  PlayerRenderer playerRenderer(&playerSystem);
  TerrainRenderer terrainRenderer(&terrain);
  ProjectileRenderer projectileRenderer(&projectileSystem);
  PowerupRenderer powerupRenderer(&powerupSystem);

  std::set<int> buttonsDown;

  // Main loop
  const float dt = 1.f/60.f; // logic tickrate
  double currentTime = glfwGetTime();
  double accumulator = 0.0;

  EventManager::Send(Event::GAME_START);

  while (!glfwWindowShouldClose(window)) {

    double newTime = glfwGetTime();
    double frameTime = newTime - currentTime;
    currentTime = newTime;
    accumulator += frameTime;

    // Logic tick
    while (accumulator >= dt) {
      accumulator -= dt;

      // Player input
      //////////////////////////////////////////

      int count;
      int joy = GLFW_JOYSTICK_1;

      // DIRTY HACK:
      // Stop my accelerometer from taking priority as the joystick
      if (glfwGetJoystickName(joy)[0] == 'S')
	joy = GLFW_JOYSTICK_2;

      const unsigned char* buttons =
	glfwGetJoystickButtons(joy, &count);

      for (int i = 0; i < count; ++i) {
	bool buttonDown = (bool)buttons[i];

	// Press event
	if (buttonDown && !buttonsDown.count(i)) {
	  buttonsDown.insert(i);
	  playerSystem.processInput(0, i, true);
	}

	// Release event
	else if (!buttonDown && buttonsDown.count(i)) {
	  buttonsDown.erase(i);
	  playerSystem.processInput(0, i, false);
	}
      }

      // Tick update
      const float* axes = glfwGetJoystickAxes(joy, &count);
      playerSystem.update(dt, axes);
      projectileSystem.update(dt);
      powerupSystem.update(dt);
      terrain.update(currentTime, dt);

      // Camera movement
      cameraSystem.update(dt);
    }

    /////////
    // Render
    /////////

    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Camera
    glm::mat4 view = cameraSystem.getView();

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
	glm::value_ptr(view));

    terrainRenderer.draw();
    playerRenderer.draw();
    projectileRenderer.draw();
    powerupRenderer.draw();

    // Post process pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_post.use();
    glBindVertexArray(screenVAO);
    glBindTexture(GL_TEXTURE_2D, FBO_texColorBuffer);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
