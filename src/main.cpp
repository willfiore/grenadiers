#include <GLFW/glfw3.h>

int main() {
  GLFWwindow* window;

  // Initialize GLFW
  if (!glfwInit()) {
    return -1;
  }

  // Create window
  window = glfwCreateWindow(640, 480, "Platformer", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  // Set current context to this window
  glfwMakeContextCurrent(window);

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
