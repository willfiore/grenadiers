#include <glad/glad.h>	  // OpenGL bindings
#include <GLFW/glfw3.h>	  // OpenGL helpers

#include <iostream>
#include <sstream>
#include <set>

#include "ResourceManager.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Player.hpp"

#include "CameraSystem.hpp"
#include "PlayerSystem.hpp"
#include "ProjectileSystem.hpp"

#include "Renderer/PlayerRenderer.hpp"
#include "Renderer/ProjectileRenderer.hpp"
#include "Renderer/TerrainRenderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {

  ////////////////////////
  // Window initialization
  ////////////////////////

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);
  glfwSwapInterval(1);

  int windowWidth = 1366;
  int windowHeight = 768;

  // Init window
  GLFWwindow* window =
    glfwCreateWindow(
	windowWidth, windowHeight,
	"Platformer",
	glfwGetPrimaryMonitor(),
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

  glEnable(GL_CULL_FACE);

  // Blending
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ////////////////////////////////////////////////
  // Projection and camera transformation matrices
  ////////////////////////////////////////////////

  glfwGetWindowSize(window, &windowWidth, &windowHeight);

  glm::mat4 projection = glm::ortho(
      0.f, (float)windowWidth,
      0.f, (float)windowHeight,
      -1.f, 1.f
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

  ResourceManager::LoadShader(
      "../assets/shaders/base.vert", "../assets/shaders/base.frag",
      "base"
      );

  Terrain terrain;

  ProjectileSystem projectileSystem(&terrain);
  PlayerSystem playerSystem(&terrain, &projectileSystem);
  CameraSystem cameraSystem(&playerSystem);
  cameraSystem.setWindowDimensions(windowWidth, windowHeight);

  PlayerRenderer playerRenderer(&playerSystem);
  TerrainRenderer terrainRenderer(&terrain);
  ProjectileRenderer projectileRenderer(&projectileSystem);

  std::set<int> buttonsDown;

  // Main loop
  const float dt = 1.f/60.f; // logic tickrate
  double currentTime = glfwGetTime();
  double accumulator = 0.0;

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
      const unsigned char* buttons =
	glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

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
      const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
      playerSystem.update(dt, axes);
      projectileSystem.update(dt);

      // Camera movement
      cameraSystem.update(dt);
    }

    /////////
    // Render
    /////////

    // glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Camera
    glm::mat4 view = cameraSystem.getView();

    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
	glm::value_ptr(view));

    terrainRenderer.draw();
    playerRenderer.draw();
    projectileRenderer.draw();

    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
	glm::value_ptr(glm::mat4()));

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
