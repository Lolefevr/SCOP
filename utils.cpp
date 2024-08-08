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
