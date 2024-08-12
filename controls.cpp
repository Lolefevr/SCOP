#include "controls.h"

float objectPosition[3] = {0.0f, 0.0f, 0.0f};
bool showColors = false;     // Pour basculer entre couleur et fil de fer blanc
bool transitioning = false;  // Pour gérer la transition douce

void processInput(GLFWwindow* window, float deltaTime) {
  const float speed = 2.5f;

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // Avant
    objectPosition[2] -= speed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // Arrière
    objectPosition[2] += speed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  // Gauche
    objectPosition[0] -= speed * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // Droite
    objectPosition[0] += speed * deltaTime;

  static bool mKeyWasPressed = false;
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
    if (!mKeyWasPressed) {
      showColors = !showColors;
      transitioning = true;  // Démarrer la transition douce
      mKeyWasPressed = true;
    }
  } else {
    mKeyWasPressed = false;
  }
}
