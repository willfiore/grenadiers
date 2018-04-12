#include <iostream>
#include <sstream>

#include <set>
#include <map>

#include <glad/glad.h>	  // OpenGL bindings
#include <GLFW/glfw3.h>	  // OpenGL helpers

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ControllerData.hpp"

#include "Console.hpp"
#include "Window.hpp"
#include "ResourceManager.hpp"
#include "Terrain.hpp"
#include "Player.hpp"

#include "EventManager.hpp"

#include "TimescaleSystem.hpp"
#include "CameraSystem.hpp"
#include "PlayerSystem.hpp"
#include "GrenadeSystem.hpp"
#include "PowerupSystem.hpp"

#include "Renderer/BaseRenderer.hpp"
#include "Renderer/PlayerRenderer.hpp"
#include "Renderer/GrenadeRenderer.hpp"
#include "Renderer/TerrainRenderer.hpp"
#include "Renderer/PowerupRenderer.hpp"
#include "Renderer/TimescaleZoneRenderer.hpp"

int main() {

  glfwInit();

  Window w;

  glEnable(GL_DEPTH_TEST);

  // ImGui
  ImGui::CreateContext();
  ImGui_ImplGlfwGL3_Init(w.getWindow(), true);
  ImGuiStyle* imguiStyle = &ImGui::GetStyle();

  imguiStyle->WindowRounding = 4.f;
  imguiStyle->WindowTitleAlign = {0.5, 0.5};
  imguiStyle->Colors[ImGuiCol_WindowBg] = {0.1, 0.1, 0.1, 0.9};
  imguiStyle->Colors[ImGuiCol_TitleBg] = {0.3, 0.3, 0.3, 0.9};
  imguiStyle->Colors[ImGuiCol_TitleBgActive] = {0.3, 0.3, 0.3, 0.9};
  imguiStyle->Colors[ImGuiCol_Header] = {0.3, 0.3, 0.3, 0.9};

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

  ResourceManager::LoadShader("base", "base.vert", "base.frag");
  ResourceManager::LoadShader("terrain", "terrain.vert", "terrain.frag");
  ResourceManager::LoadShader("post", "screen.vert", "post.frag");
  ResourceManager::LoadShader("blur", "screen.vert", "blur.frag");
  ResourceManager::LoadShader("bg_mesh", "terrain.vert", "bg_mesh.frag");
  ResourceManager::LoadShader("inertia_zone", "base.vert", "inertia_zone.frag");
  w.initShaders();

  // std::vector<glm::vec3> cm_v;
  // std::vector<unsigned int> cm_i;
  // // Circle
  // unsigned int circleNumPoints = 128;
  // float divisionSize = glm::two_pi<float>() / circleNumPoints;
  // cm_v.push_back({0.f, 0.f, 0.f});
  // for (unsigned int i = 0; i < circleNumPoints; ++i) {
  //   float angle = i*divisionSize;
  //   // v is i+1
  //   cm_v.push_back({glm::cos(angle), glm::sin(angle), 0.f});
  //   // next v is i+2
  //   unsigned int v1 = i+1;
  //   unsigned int v2 = i+2;
  //   if (v2 > circleNumPoints) v2 = 1;
  //   cm_i.insert(cm_i.end(), {0, v1, v2});
  // }

  // Model cm(cm_v, cm_i);
  // ResourceManager::SaveModel(cm, "circle.model");

  ResourceManager::LoadModel("quad", "quad.model");
  ResourceManager::LoadModel("circle", "circle.model");

  // Controller setup
  std::map<int, ControllerData> controllers;

  for (int i = 0; i <= GLFW_JOYSTICK_LAST; ++i) {
    if (!glfwJoystickPresent(i)) continue;

    // Dirty hack to ignore my laptop accelerometer
    if (glfwGetJoystickName(i)[1] == 'T') continue;

    controllers[i] = ControllerData();
  }

  // Module setup
  TimescaleSystem timescaleSystem;

  Terrain terrain;
  PlayerSystem playerSystem(terrain, controllers, timescaleSystem);
  GrenadeSystem grenadeSystem(terrain, timescaleSystem, playerSystem);
  PowerupSystem powerupSystem(terrain, playerSystem);
  CameraSystem cameraSystem(&w, playerSystem.getPlayers());

  PlayerRenderer playerRenderer(playerSystem);
  TerrainRenderer terrainRenderer(terrain);
  GrenadeRenderer grenadeRenderer(grenadeSystem);
  PowerupRenderer powerupRenderer(powerupSystem);
  TimescaleZoneRenderer timescaleZoneRenderer(timescaleSystem);


  // Main loop
  const double dt = 1.f/60.f; // logic tickrate

  double t = glfwGetTime();
  double sim_t = 0.f;
  double accumulator = 0.0;

  EventManager::Send(Event::GAME_START);

  while (!glfwWindowShouldClose(w.getWindow())) {

    ImGui_ImplGlfwGL3_NewFrame();

    Console::render();

    double newTime = glfwGetTime();
    double frameTime = newTime - t;
    t = newTime;
    accumulator += frameTime;

    glfwPollEvents();

    // Logic tick
    // WARNING: "if" rather than "while" can cause spiral of death
    if (accumulator >= dt) {
      accumulator -= dt;

      double sim_dt = timescaleSystem.getGlobalTimescale() * dt;
      sim_t += sim_dt;

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
      EventManager::Update(sim_t, sim_dt);
      timescaleSystem.update(t, dt);

      grenadeSystem.update(sim_dt);
      powerupSystem.update(sim_dt);
      terrain.update(sim_t, sim_dt);
      playerSystem.update(sim_t, sim_dt);

      // Camera movement
      cameraSystem.update(sim_t, sim_dt);
    }

    /////////
    // Render
    /////////

    // First pass
    glBindFramebuffer(GL_FRAMEBUFFER, w.getFBO());
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.6f, 0.1f, 0.0f, 0.f);
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
    grenadeRenderer.draw();
    powerupRenderer.draw();
    timescaleZoneRenderer.draw();

    // --------------------------------
    // Finished rendering scene
    // Do not need depth test in post

    // Final pass to screen
    w.render();

    ImGui::Render();
    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(w.getWindow());
  }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
