#ifndef SHADER_HPP
#define SHADER_HPP

#include <GLFW/glfw3.h>
#include <string>


std::string loadShaderSource(const char* filepath);
GLuint compileShader(GLenum type, const std::string& source);
GLuint loadShaders(const char* vertexPath, const char* fragmentPath);

void loadOpenGLFunctions();

#endif
