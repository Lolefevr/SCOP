#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "shader.hpp"

std::vector<glm::vec3> vertices;
std::vector<unsigned int> indices;
std::vector<glm::vec3> normals;
std::vector<glm::vec3> colors; // Pour les couleurs des sommets
glm::vec3 objectPosition(0.0f, 0.0f, 0.0f);
float rotationX = 0.0f;

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

void loadOBJ(const std::string& path) {
    std::ifstream objFile(path);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open .obj file: " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
            colors.push_back(glm::vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f)); // Couleur aléatoire pour chaque sommet
        } else if (prefix == "f") {
            std::vector<unsigned int> vertexIndices;
            std::string vertexData;
            while (iss >> vertexData) {
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::istringstream viss(vertexData);
                unsigned int vertexIndex;
                viss >> vertexIndex;
                vertexIndices.push_back(vertexIndex - 1);
            }
            for (size_t i = 1; i < vertexIndices.size() - 1; ++i) {
                indices.push_back(vertexIndices[0]);
                indices.push_back(vertexIndices[i]);
                indices.push_back(vertexIndices[i + 1]);
            }
        }
    }

    objFile.close();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
	(void)window;
}

glm::vec3 calculateCentroid(const std::vector<glm::vec3>& vertices) {
    glm::vec3 centroid(0.0f, 0.0f, 0.0f);
    for (const auto& vertex : vertices) {
        centroid += vertex;
    }
    centroid /= static_cast<float>(vertices.size());
    return centroid;
}

void centerVertices(std::vector<glm::vec3>& vertices, const glm::vec3& centroid) {
    for (auto& vertex : vertices) {
        vertex -= centroid;
    }
}

void calculateNormals(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<glm::vec3>& normals) {
    normals.resize(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
    for (size_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 v0 = vertices[indices[i]];
        glm::vec3 v1 = vertices[indices[i + 1]];
        glm::vec3 v2 = vertices[indices[i + 2]];
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        normals[indices[i]] += normal;
        normals[indices[i + 1]] += normal;
        normals[indices[i + 2]] += normal;
    }
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Project", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Charger la texture
    GLuint texture = loadTexture("../textures/cat.jpg");

    // Charger l'objet .obj
    loadOBJ("../models/42.obj");

    // Calcul du centre de l'objet et recentrage
    glm::vec3 centroid = calculateCentroid(vertices);
    centerVertices(vertices, centroid);

    // Calcul des normales
    calculateNormals(vertices, indices, normals);

    GLuint VBO, VAO, EBO, NBO, colorVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Charger les shaders
    GLuint shaderProgram = loadShaders("../shaders/vertex_shader.glsl", "../shaders/fragment_shader.glsl");
    GLuint grayscaleShaderProgram = loadShaders("../shaders/vertex_shader.glsl", "../shaders/fragment_shader_grayscale.glsl");
    GLuint pointShaderProgram = loadShaders("../shaders/vertex_shader.glsl", "../shaders/fragment_shader_point.glsl");
    GLuint phongShaderProgram = loadShaders("../shaders/vertex_shader.glsl", "../shaders/fragment_shader_phong.glsl");
    GLuint flatShaderProgram = loadShaders("../shaders/vertex_shader_flat.glsl", "../shaders/fragment_shader_flat.glsl");
    GLuint wireframeColorShaderProgram = loadShaders("../shaders/vertex_shader_color.glsl", "../shaders/fragment_shader_color.glsl");
	GLuint transitionShaderProgram = loadShaders("../shaders/vertex_shader_transition.glsl", "../shaders/fragment_shader_transition.glsl");

    GLuint currentShaderProgram = shaderProgram;
    bool useTexture = false;
    bool transitioning = false;
    bool showingTexture = false;
    float mixFactor = 0.0f;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
    glm::mat4 model = glm::mat4(1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            objectPosition.y += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            objectPosition.y -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            objectPosition.x -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            objectPosition.x += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            objectPosition.z += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            objectPosition.z -= 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            useTexture = !useTexture;
        }

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !transitioning) {
            transitioning = true;
            showingTexture = !showingTexture;  // Alterne entre couleur et texture
        }

        // Transition douce
        if (transitioning) {
            if (showingTexture) {
                mixFactor += 0.001f;  // Transition vers la texture
                if (mixFactor >= 1.0f) {
                    mixFactor = 1.0f;
                    transitioning = false;
                }
            } else {
                mixFactor -= 0.001f;  // Transition vers les couleurs
                if (mixFactor <= 0.0f) {
                    mixFactor = 0.0f;
                    transitioning = false;
                }
            }
            currentShaderProgram = transitionShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Sélection du shader
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            currentShaderProgram = grayscaleShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            currentShaderProgram = shaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
            currentShaderProgram = pointShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glPointSize(2.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
            currentShaderProgram = phongShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
            currentShaderProgram = flatShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
            currentShaderProgram = wireframeColorShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            rotationX += 0.01f;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            rotationX -= 0.01f;
        }

        // Mise à jour des transformations
        model = glm::translate(glm::mat4(1.0f), objectPosition);
        model = glm::rotate(model, rotationX, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glUseProgram(currentShaderProgram);

        GLint viewLoc = glGetUniformLocation(currentShaderProgram, "view");
        GLint projLoc = glGetUniformLocation(currentShaderProgram, "projection");
        GLint modelLoc = glGetUniformLocation(currentShaderProgram, "model");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Uniformes pour le Phong Shading
        if (currentShaderProgram == phongShaderProgram) {
            GLint lightPosLoc = glGetUniformLocation(currentShaderProgram, "lightPos");
            GLint viewPosLoc = glGetUniformLocation(currentShaderProgram, "viewPos");
            GLint lightColorLoc = glGetUniformLocation(currentShaderProgram, "lightColor");
            GLint objectColorLoc = glGetUniformLocation(currentShaderProgram, "objectColor");

            glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);
            glUniform3f(viewPosLoc, 0.0f, 0.0f, 3.0f);
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
            glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        }

        // Uniformes pour le Flat Shading
        if (currentShaderProgram == flatShaderProgram) {
            GLint lightPosLoc = glGetUniformLocation(currentShaderProgram, "lightPos");
            GLint lightColorLoc = glGetUniformLocation(currentShaderProgram, "lightColor");
            GLint objectColorLoc = glGetUniformLocation(currentShaderProgram, "objectColor");

            glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);
            glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
            glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
        }

        if (currentShaderProgram == transitionShaderProgram) {
            GLint mixFactorLoc = glGetUniformLocation(currentShaderProgram, "mixFactor");
            glUniform1f(mixFactorLoc, mixFactor);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            GLint textureLoc = glGetUniformLocation(currentShaderProgram, "ourTexture");
            glUniform1i(textureLoc, 0);
        }

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Nettoyage
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &colorVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
