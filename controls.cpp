#include "controls.h"

glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
bool showColors = false; // Pour basculer entre couleur et fil de fer blanc

void processInput(GLFWwindow* window, float deltaTime) {
    const float speed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // Avant
        objectPosition.z -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) // Arrière
        objectPosition.z += speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) // Gauche
        objectPosition.x -= speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) // Droite
        objectPosition.x += speed * deltaTime;

    static bool mKeyWasPressed = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!mKeyWasPressed) {
            showColors = !showColors;
            mKeyWasPressed = true;
        }
    } else {
        mKeyWasPressed = false;
    }
}
