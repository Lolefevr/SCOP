#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

// Variables globales pour la position de l'objet
extern glm::vec3 objectPosition;
extern bool showColors;
extern bool transitioning;  // Déclaration de la variable extern

void processInput(GLFWwindow* window, float deltaTime);

#endif  // CONTROLS_H
