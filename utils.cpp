#include "utils.h"
#include <iostream>

int windowWidth = 800;
int windowHeight = 600;
bool showColors = false; // Pour basculer entre couleur et fil de fer blanc

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window; // Supprime l'avertissement pour le paramètre non utilisé
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) // Touche pour basculer entre couleur et fil de fer blanc
        showColors = !showColors;
}
