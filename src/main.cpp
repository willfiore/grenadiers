
#include <iostream>
#include <sstream>

#include <set>
#include <map>

#include <glad/glad.h>	  // OpenGL bindings
#include <GLFW/glfw3.h>	  // OpenGL helpers

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ControllerData.hpp"

#include "Window.hpp"
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

  /////////////////
  // Initialization
  /////////////////

  glfwInit();

  Window w;
  glEnable(GL_DEPTH_TEST);

  ////////////////////////////////////////////////
  // Projection and camera transformation matrices
  ////////////////////////////////////////////////

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
  CameraSystem cameraSystem(&w, playerSystem.getPlayers());

  PlayerRenderer playerRenderer(&playerSystem);
  TerrainRenderer terrainRenderer(&terrain);
  ProjectileRenderer projectileRenderer(&projectileSystem);
  PowerupRenderer powerupRenderer(&powerupSystem);

  // Main loop
  const float dt = 1.f/60.f; // logic tickrate
  double currentTime = glfwGetTime();
  double accumulator = 0.0;

  EventManager::Send(Event::GAME_START);

  while (!glfwWindowShouldClose(w.getWindow())) {

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
    glBindFramebuffer(GL_FRAMEBUFFER, w.getFBO());
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

    // Final pass to screen
    shader_post.use();
    w.render();

    glfwSwapBuffers(w.getWindow());
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
