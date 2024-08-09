#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "obj_loader.h"
#include "shaders.h"
#include "utils.h"
#include "controls.h"
#define STB_IMAGE_IMPLEMENTATION
#include "./include/stb_image.h"

float mixValue = 0.0f; // Départ en mode fil de fer
float transitionSpeed = 0.5f; // Vitesse de transition ajustée
int state = 0; // 0: wireframe, 1: colored faces, 3: textured faces

extern bool transitioning; // Utilisation de la variable extern
extern bool showColors; // Utilisation de la variable extern

void loadTexture(const char* path, GLuint& textureID) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Project", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Activer le test de profondeur
    glEnable(GL_DEPTH_TEST);

    // Activer le blending pour les textures avec alpha
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Définir la fonction de rappel pour le redimensionnement de la fenêtre
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Charger l'objet .obj
    loadOBJ("42.obj");

    // Calculer et centrer les sommets de l'objet
    glm::vec3 centroid = calculateCentroid(vertices);
    centerVertices(vertices, centroid);

    // Charger la texture
    GLuint textureID;
    loadTexture("./textures/texture.jpg", textureID);

    // Créer les VBO et VAO
    GLuint VBO, VAO, EBO, colorVBO, texVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &colorVBO);
    glGenBuffers(1, &texVBO);

    glBindVertexArray(VAO);

    // VBO pour les sommets
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // VBO pour les couleurs
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    // VBO pour les coordonnées de texture
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(2);

    // EBO pour les indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Charger et compiler les shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Vérifier les erreurs de compilation du vertex shader
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Vérifier les erreurs de compilation du fragment shader
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Vérifier les erreurs de linkage du programme
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Variables pour le chronométrage
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Positions de la lumière et de la caméra pour le shader figurine
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::vec3 viewPos(0.0f, 0.0f, 3.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    // Boucle de rendu
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        if (state == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Fil de fer
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Rempli
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLuint mixValueLoc = glGetUniformLocation(shaderProgram, "mixValue");
        GLuint stateLoc = glGetUniformLocation(shaderProgram, "state");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(mixValueLoc, mixValue);
        glUniform1i(stateLoc, state);

        // Passer les uniformes supplémentaires pour l'effet figurine
        if (state == 2) {
            GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
            GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
            GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
            GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");

            glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
            glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));
            glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
            glUniform3fv(objectColorLoc, 1, glm::value_ptr(objectColor));
        }

        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();

// Gestion de la transition douce
if (transitioning) {
    if (showColors) {
        mixValue += transitionSpeed * deltaTime;
        if (mixValue >= 1.0f) {
            mixValue = 1.0f;
            if (state == 1) {
                state = 2; // Passer à l'état texturé
                transitioning = false;
            } else {
                state++;
            }
        } else if (state == 0 && mixValue > 0.0f) {
            state = 1; // Passer à l'état faces colorées
        }
    } else {
        mixValue -= transitionSpeed * deltaTime;
        if (mixValue <= 0.0f) {
            mixValue = 0.0f;
            state = 0; // Revenir à l'état fil de fer
            transitioning = false;
        } else if (state == 2 && mixValue < 1.0f) {
            state = 1; // Revenir à l'état faces colorées
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
