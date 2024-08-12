#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>

#include "controls.h"
#include "obj_loader.h"
#include "shaders.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./include/stb_image.h"

// Déclarations de pointeurs de fonction pour les fonctions OpenGL
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLUNIFORM3FVPROC glUniform3fv = nullptr;

float mixValue = 0.0f;
float transitionSpeed = 0.5f;
int state = 0;

extern bool transitioning;
extern bool showColors;

void loadTexture(const char* path, GLuint& textureID) {
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  int width, height, nrChannels;
  unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);
}

void identityMatrix(float* matrix) {
  for (int i = 0; i < 16; ++i) {
    matrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
  }
}

void translateMatrix(float* matrix, float x, float y, float z) {
  identityMatrix(matrix);
  matrix[12] = x;
  matrix[13] = y;
  matrix[14] = z;
}

void rotateMatrixY(float* matrix, float angle) {
  float c = cos(angle);
  float s = sin(angle);
  identityMatrix(matrix);
  matrix[0] = c;
  matrix[2] = -s;
  matrix[8] = s;
  matrix[10] = c;
}

void multiplyMatrices(float* result, const float* a, const float* b) {
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result[i * 4 + j] =
          a[i * 4 + 0] * b[0 * 4 + j] + a[i * 4 + 1] * b[1 * 4 + j] +
          a[i * 4 + 2] * b[2 * 4 + j] + a[i * 4 + 3] * b[3 * 4 + j];
    }
  }
}

void perspectiveMatrix(float* matrix, float fov, float aspect, float near,
                       float far) {
  float tanHalfFov = tan(fov / 2.0f);
  identityMatrix(matrix);
  matrix[0] = 1.0f / (aspect * tanHalfFov);
  matrix[5] = 1.0f / tanHalfFov;
  matrix[10] = -(far + near) / (far - near);
  matrix[11] = -1.0f;
  matrix[14] = -(2.0f * far * near) / (far - near);
  matrix[15] = 0.0f;
}

void loadOpenGLFunctions() {
  glGenVertexArrays =
      (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
  glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
  glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
  glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
  glVertexAttribPointer =
      (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
  glEnableVertexAttribArray =
      (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress(
          "glEnableVertexAttribArray");
  glBindVertexArray =
      (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
  glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
  glUniformMatrix4fv =
      (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");
  glUniform1f = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
  glUniform1i = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");
  glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
  glDeleteBuffers =
      (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
  glDeleteVertexArrays =
      (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
  glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
  glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
  glCompileShader =
      (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
  glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
  glGetShaderInfoLog =
      (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
  glCreateProgram =
      (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
  glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
  glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
  glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
  glGetProgramInfoLog =
      (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
  glGetUniformLocation =
      (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
  glGenerateMipmap =
      (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");
  glUniform3fv = (PFNGLUNIFORM3FVPROC)glfwGetProcAddress("glUniform3fv");

  if (!glGenVertexArrays || !glGenBuffers || !glBindBuffer || !glBufferData ||
      !glVertexAttribPointer || !glEnableVertexAttribArray ||
      !glBindVertexArray || !glUseProgram || !glUniformMatrix4fv ||
      !glUniform1f || !glUniform1i || !glDeleteShader || !glDeleteBuffers ||
      !glDeleteVertexArrays || !glCreateShader || !glShaderSource ||
      !glCompileShader || !glGetShaderiv || !glGetShaderInfoLog ||
      !glCreateProgram || !glAttachShader || !glLinkProgram ||
      !glGetProgramiv || !glGetProgramInfoLog || !glGetUniformLocation ||
      !glGenerateMipmap || !glUniform3fv) {
    std::cerr << "Failed to load OpenGL functions." << std::endl;
    glfwTerminate();
    exit(-1);
  }
}

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return -1;
  }

  GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight,
                                        "OpenGL Project", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  loadOpenGLFunctions();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  loadOBJ("422.obj");

  float centroid[3] = {0.0f, 0.0f, 0.0f};
  calculateCentroid(vertices, centroid);
  centerVertices(vertices, centroid);

  GLuint textureID;
  loadTexture("./textures/texture.jpg", textureID);

  GLuint VBO, VAO, EBO, colorVBO, texVBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  glGenBuffers(1, &colorVBO);
  glGenBuffers(1, &texVBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * 3 * sizeof(float),
               colors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, texVBO);
  glBufferData(GL_ARRAY_BUFFER, texCoords.size() * 2 * sizeof(float),
               texCoords.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
               indices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  float lightPos[3] = {1.2f, 1.0f, 2.0f};
  float viewPos[3] = {0.0f, 0.0f, 3.0f};
  float lightColor[3] = {1.0f, 1.0f, 1.0f};
  float objectColor[3] = {1.0f, 0.5f, 0.31f};

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window, deltaTime);

    if (state == 0) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    float rotation[16];
    rotateMatrixY(rotation, glfwGetTime());

    float translation[16];
    translateMatrix(translation, objectPosition[0], objectPosition[1],
                    objectPosition[2]);

    float model[16];
    multiplyMatrices(model, rotation, translation);

    float view[16];
    translateMatrix(view, 0.0f, 0.0f, -5.0f);

    float projection[16];
    perspectiveMatrix(projection, 45.0f * 3.14159265358979f / 180.0f,
                      (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    GLuint mixValueLoc = glGetUniformLocation(shaderProgram, "mixValue");
    GLuint stateLoc = glGetUniformLocation(shaderProgram, "state");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, projection);
    glUniform1f(mixValueLoc, mixValue);
    glUniform1i(stateLoc, state);

    if (state == 2) {
      GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
      GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
      GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
      GLuint objectColorLoc =
          glGetUniformLocation(shaderProgram, "objectColor");

      glUniform3fv(lightPosLoc, 1, lightPos);
      glUniform3fv(viewPosLoc, 1, viewPos);
      glUniform3fv(lightColorLoc, 1, lightColor);
      glUniform3fv(objectColorLoc, 1, objectColor);
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();

    if (transitioning) {
      if (showColors) {
        mixValue += transitionSpeed * deltaTime;
        if (mixValue >= 1.0f) {
          mixValue = 1.0f;
          if (state == 1) {
            state = 2;
            transitioning = false;
          } else {
            state++;
          }
        } else if (state == 0 && mixValue > 0.0f) {
          state = 1;
        }
      } else {
        mixValue -= transitionSpeed * deltaTime;
        if (mixValue <= 0.0f) {
          mixValue = 0.0f;
          state = 0;
          transitioning = false;
        } else if (state == 2 && mixValue < 1.0f) {
          state = 1;
        }
      }
    }
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteBuffers(1, &colorVBO);
  glDeleteBuffers(1, &texVBO);

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
