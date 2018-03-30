#include <iostream>
#include <sstream>

#include <set>
#include <map>

#include <glad/glad.h>	  // OpenGL bindings
#include <GLFW/glfw3.h>	  // OpenGL helpers

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ControllerData.hpp"

#include "ResourceManager.hpp"
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

  // Initialize space in UBO
  unsigned int UBO;
  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferRange(GL_UNIFORM_BUFFER, 0, UBO, 0, sizeof(glm::mat4));

  // Push initial matrices to UBO
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
      glm::value_ptr(glm::mat4()));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // -----------------------------------

  // Create FBO for initial render
  unsigned int FBO;
  unsigned int FBO_buffer;
  glGenFramebuffers(1, &FBO);
  glGenTextures(1, &FBO_buffer);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  glBindTexture(GL_TEXTURE_2D, FBO_buffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight,
      0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
      FBO_buffer, 0);
  // We require a depth buffer here
  unsigned int FBO_RBO;
  glGenRenderbuffers(1, &FBO_RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, FBO_RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
      windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
      GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, FBO_RBO);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Create ping-pong FBOs for post
  unsigned int ppFBO[2];
  unsigned int ppFBO_buffer[2];
  glGenFramebuffers(2, ppFBO);
  glGenTextures(2, ppFBO_buffer);

  for (unsigned int i = 0; i < 2; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, ppFBO[i]);
    glBindTexture(GL_TEXTURE_2D, ppFBO_buffer[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight,
	0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
	ppFBO_buffer[i], 0);
  }

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

  Shader shader_pingpong = ResourceManager::LoadShader(
      "../assets/shaders/pingpong.vert", "../assets/shaders/pingpong.frag",
      "pingpong"
      );

  shader_post.use();
  shader_post.setInt("screenTexture", 0);

  shader_pingpong.use();
  shader_pingpong.setInt("screenTexture", 0);


  // Controller setup
  std::map<int, ControllerData> controllers;

  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    if (!glfwJoystickPresent(i)) continue;

    // Dirty hack to ignore my laptop accelerometer
    if (glfwGetJoystickName(i)[1] == 'T') continue;

    controllers[i] = ControllerData();
  }

  // Module setup
  Terrain terrain;
  PlayerSystem playerSystem(&terrain, &controllers);
  ProjectileSystem projectileSystem(&terrain);
  PowerupSystem powerupSystem(&terrain, &playerSystem);
  CameraSystem cameraSystem(playerSystem.getPlayers());
  cameraSystem.setWindowDimensions(windowWidth, windowHeight);

  PlayerRenderer playerRenderer(&playerSystem);
  TerrainRenderer terrainRenderer(&terrain);
  ProjectileRenderer projectileRenderer(&projectileSystem);
  PowerupRenderer powerupRenderer(&powerupSystem);

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

      for (auto& p : controllers) {

	int count;
	const unsigned char* buttons =
	  glfwGetJoystickButtons(p.first, &count);

	for (int i = 0; i < count; ++i) {
	  bool buttonDown = (bool)buttons[i];

	  // Press event
	  if (buttonDown && !p.second.buttons.count(i)) {
	    p.second.buttons.insert(i);
	    playerSystem.processInput(p.first, i, true);
	  }

	  // Release event
	  else if (!buttonDown && p.second.buttons.count(i)) {
	    p.second.buttons.erase(i);
	    playerSystem.processInput(p.first, i, false);
	  }
	}

	const float* axes = glfwGetJoystickAxes(p.first, &count);
	p.second.axes.assign(axes, axes + count);
      }

      // Tick update
      playerSystem.update(dt);
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
    glm::mat4 projection = cameraSystem.getProjection();
    glm::mat4 view = cameraSystem.getView();

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
	glm::value_ptr(projection));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
	glm::value_ptr(view));

    terrainRenderer.draw();
    playerRenderer.draw();
    projectileRenderer.draw();
    powerupRenderer.draw();

    // --------------------------------
    // Finished rendering scene
    // Do not need depth test in post
    glDisable(GL_DEPTH_TEST);

    // Intermediate ping-pong buffer
    glBindFramebuffer(GL_FRAMEBUFFER, ppFBO[0]);
    glBindTexture(GL_TEXTURE_2D, FBO_buffer);

    shader_pingpong.use();
    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Final pass to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, ppFBO_buffer[0]);

    shader_post.use();
    glBindVertexArray(screenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
