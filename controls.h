#ifndef CONTROLS_H
#define CONTROLS_H

#include <GLFW/glfw3.h>

extern float objectPosition[3];
extern bool showColors;
extern bool transitioning;

void processInput(GLFWwindow* window, float deltaTime);

#endif  // CONTROLS_H
