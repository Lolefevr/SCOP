#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>

struct Vec3 {
    float x, y, z;
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Mat4 {
    float elements[16] = {0};

    static Mat4 identity() {
        Mat4 result;
        for (int i = 0; i < 4; i++)
            result.elements[i + i * 4] = 1.0f;
        return result;
    }

    static Mat4 translate(const Vec3& position) {
        Mat4 result = Mat4::identity();
        result.elements[12] = position.x;
        result.elements[13] = position.y;
        result.elements[14] = position.z;
        return result;
    }

    static Mat4 rotate(float angle, const Vec3& axis) {
        Mat4 result = Mat4::identity();
        float r = angle;
        float c = cos(r);
        float s = sin(r);

        result.elements[0] = axis.x * axis.x * (1.0f - c) + c;
        result.elements[1] = axis.x * axis.y * (1.0f - c) - axis.z * s;
        result.elements[2] = axis.x * axis.z * (1.0f - c) + axis.y * s;

        result.elements[4] = axis.y * axis.x * (1.0f - c) + axis.z * s;
        result.elements[5] = axis.y * axis.y * (1.0f - c) + c;
        result.elements[6] = axis.y * axis.z * (1.0f - c) - axis.x * s;

        result.elements[8] = axis.z * axis.x * (1.0f - c) - axis.y * s;
        result.elements[9] = axis.z * axis.y * (1.0f - c) + axis.x * s;
        result.elements[10] = axis.z * axis.z * (1.0f - c) + c;

        return result;
    }

    static Mat4 perspective(float fov, float aspectRatio, float near, float far) {
        Mat4 result;
        float q = 1.0f / tan(0.5f * fov);
        float a = q / aspectRatio;

        float b = (near + far) / (near - far);
        float c = (2.0f * near * far) / (near - far);

        result.elements[0] = a;
        result.elements[5] = q;
        result.elements[10] = b;
        result.elements[11] = -1.0f;
        result.elements[14] = c;

        return result;
    }

    // Surcharge de l'opérateur * pour la multiplication matricielle
    Mat4 operator*(const Mat4& other) const {
        Mat4 result;

        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                result.elements[col + row * 4] =
                    elements[0 + row * 4] * other.elements[col + 0 * 4] +
                    elements[1 + row * 4] * other.elements[col + 1 * 4] +
                    elements[2 + row * 4] * other.elements[col + 2 * 4] +
                    elements[3 + row * 4] * other.elements[col + 3 * 4];
            }
        }

        return result;
    }
};

// Déclaration des pointeurs de fonction pour OpenGL
PFNGLCREATESHADERPROC glCreateShader = nullptr;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
PFNGLBUFFERDATAPROC glBufferData = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = nullptr;
PFNGLDELETESHADERPROC glDeleteShader = nullptr;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
PFNGLATTACHSHADERPROC glAttachShader = nullptr;
PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
PFNGLGETPROGRAMIVPROC glGetProgramiv = nullptr;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = nullptr;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = nullptr;
PFNGLUNIFORM1IPROC glUniform1i = nullptr;
PFNGLUNIFORM1FPROC glUniform1f = nullptr;
PFNGLUNIFORM3FPROC glUniform3f = nullptr;

void loadOpenGLFunctions() {
	glUniform3f = (PFNGLUNIFORM3FPROC)glfwGetProcAddress("glUniform3f");
	glUniform1f = (PFNGLUNIFORM1FPROC)glfwGetProcAddress("glUniform1f");
	glUniform1i = (PFNGLUNIFORM1IPROC)glfwGetProcAddress("glUniform1i");
    glCreateShader = (PFNGLCREATESHADERPROC)glfwGetProcAddress("glCreateShader");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)glfwGetProcAddress("glDeleteProgram");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
    glGenBuffers = (PFNGLGENBUFFERSPROC)glfwGetProcAddress("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)glfwGetProcAddress("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)glfwGetProcAddress("glBufferData");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)glfwGetProcAddress("glVertexAttribPointer");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)glfwGetProcAddress("glEnableVertexAttribArray");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
    glUseProgram = (PFNGLUSEPROGRAMPROC)glfwGetProcAddress("glUseProgram");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)glfwGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)glfwGetProcAddress("glUniformMatrix4fv");
    glDeleteShader = (PFNGLDELETESHADERPROC)glfwGetProcAddress("glDeleteShader");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)glfwGetProcAddress("glDeleteBuffers");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)glfwGetProcAddress("glDeleteVertexArrays");
    glShaderSource = (PFNGLSHADERSOURCEPROC)glfwGetProcAddress("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)glfwGetProcAddress("glCompileShader");
    glGetShaderiv = (PFNGLGETSHADERIVPROC)glfwGetProcAddress("glGetShaderiv");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)glfwGetProcAddress("glGetShaderInfoLog");
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)glfwGetProcAddress("glCreateProgram");
    glAttachShader = (PFNGLATTACHSHADERPROC)glfwGetProcAddress("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)glfwGetProcAddress("glLinkProgram");
    glGetProgramiv = (PFNGLGETPROGRAMIVPROC)glfwGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)glfwGetProcAddress("glGetProgramInfoLog");
    glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)glfwGetProcAddress("glGenerateMipmap");

    if (!glGenVertexArrays || !glGenBuffers || !glBindBuffer || !glBufferData || !glVertexAttribPointer ||
        !glEnableVertexAttribArray || !glBindVertexArray || !glUseProgram || !glGetUniformLocation ||
        !glUniformMatrix4fv || !glDeleteShader || !glDeleteBuffers || !glDeleteVertexArrays ||
        !glShaderSource || !glCompileShader || !glGetShaderiv || !glGetShaderInfoLog ||
        !glCreateProgram || !glAttachShader || !glLinkProgram || !glGetProgramiv ||
        !glGetProgramInfoLog || !glGenerateMipmap || !glCreateShader || !glDeleteProgram || !glUniform1i || !glUniform1f || !glUniform3f) {
        std::cerr << "Failed to load OpenGL functions." << std::endl;
        glfwTerminate();
        exit(-1);
    }
}

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        std::cerr << "Error: Failed to create shader of type " << type << std::endl;
        return 0;
    }
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint loadShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = loadShaderSource(vertexPath);
    std::string fragmentCode = loadShaderSource(fragmentPath);

    if (vertexCode.empty() || fragmentCode.empty()) {
        std::cerr << "Failed to load shader source code." << std::endl;
        return 0;
    }

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    if (vertexShader == 0) {
        std::cerr << "Error: Vertex shader compilation failed" << std::endl;
        return 0;
    }
    std::cerr << "Vertex shader compiled successfully." << std::endl;

    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    if (fragmentShader == 0) {
        std::cerr << "Error: Fragment shader compilation failed" << std::endl;
        return 0;
    }
    std::cerr << "Fragment shader compiled successfully." << std::endl;

    GLuint shaderProgram = glCreateProgram();
    if (shaderProgram == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
        return 0;
    }
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    std::cerr << "Shader program linked successfully." << std::endl;

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

std::vector<Vec3> vertices;
std::vector<unsigned int> indices;
std::vector<Vec3> normals;
std::vector<Vec3> colors;
Vec3 objectPosition(0.0f, 0.0f, 0.0f);
Vec3 cameraPosition(0.0f, 0.0f, -5.0f);
float rotationX = 0.0f, rotationY = 0.0f, rotationZ = 0.0f;
float continuousRotationAngle = 0.0f;

// Ajout des variables pour les shaders supplémentaires
GLuint grayscaleShaderProgram, pointShaderProgram, phongShaderProgram, flatShaderProgram, wireframeColorShaderProgram, transitionShaderProgram;
GLuint currentShaderProgram;
bool useTexture = false;
bool transitioning = false;
bool showingTexture = false;
float mixFactor = 0.0f;

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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
            Vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
            colors.push_back(Vec3((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f));
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

Vec3 calculateCentroid(const std::vector<Vec3>& vertices) {
    Vec3 centroid(0.0f, 0.0f, 0.0f);
    for (const auto& vertex : vertices) {
        centroid.x += vertex.x;
        centroid.y += vertex.y;
        centroid.z += vertex.z;
    }
    centroid.x /= static_cast<float>(vertices.size());
    centroid.y /= static_cast<float>(vertices.size());
    centroid.z /= static_cast<float>(vertices.size());
    return centroid;
}

void centerVertices(std::vector<Vec3>& vertices, const Vec3& centroid) {
    for (auto& vertex : vertices) {
        vertex.x -= centroid.x;
        vertex.y -= centroid.y;
        vertex.z -= centroid.z;
    }
}

void calculateNormals(const std::vector<Vec3>& vertices, const std::vector<unsigned int>& indices, std::vector<Vec3>& normals) {
    normals.resize(vertices.size(), Vec3(0.0f, 0.0f, 0.0f));
    for (size_t i = 0; i < indices.size(); i += 3) {
        Vec3 v0 = vertices[indices[i]];
        Vec3 v1 = vertices[indices[i + 1]];
        Vec3 v2 = vertices[indices[i + 2]];
        Vec3 normal;
        normal.x = (v1.y - v0.y) * (v2.z - v0.z) - (v1.z - v0.z) * (v2.y - v0.y);
        normal.y = (v1.z - v0.z) * (v2.x - v0.x) - (v1.x - v0.x) * (v2.z - v0.z);
        normal.z = (v1.x - v0.x) * (v2.y - v0.y) - (v1.y - v0.y) * (v2.x - v0.x);
        float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;

        normals[indices[i]].x += normal.x;
        normals[indices[i]].y += normal.y;
        normals[indices[i]].z += normal.z;

        normals[indices[i + 1]].x += normal.x;
        normals[indices[i + 1]].y += normal.y;
        normals[indices[i + 1]].z += normal.z;

        normals[indices[i + 2]].x += normal.x;
        normals[indices[i + 2]].y += normal.y;
        normals[indices[i + 2]].z += normal.z;
    }
    for (auto& normal : normals) {
        float length = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        normal.x /= length;
        normal.y /= length;
        normal.z /= length;
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
    loadOpenGLFunctions();  // Charge les fonctions OpenGL

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, 800, 600);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    GLuint texture = loadTexture("textures/cat.jpg");
    loadOBJ("models/42.obj");

    Vec3 centroid = calculateCentroid(vertices);
    centerVertices(vertices, centroid);
    calculateNormals(vertices, indices, normals);

    GLuint VBO, VAO, EBO, NBO, colorVBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3), &normals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(Vec3), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3), (void*)0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint shaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
    grayscaleShaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader_grayscale.glsl");
    pointShaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader_point.glsl");
    phongShaderProgram = loadShaders("shaders/vertex_shader.glsl", "shaders/fragment_shader_phong.glsl");
    flatShaderProgram = loadShaders("shaders/vertex_shader_flat.glsl", "shaders/fragment_shader_flat.glsl");
    wireframeColorShaderProgram = loadShaders("shaders/vertex_shader_color.glsl", "shaders/fragment_shader_color.glsl");
    transitionShaderProgram = loadShaders("shaders/vertex_shader_transition.glsl", "shaders/fragment_shader_transition.glsl");

    currentShaderProgram = shaderProgram;

    Mat4 projection = Mat4::perspective(45.0f * 3.14159265358979323846f / 180.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    Mat4 view = Mat4::translate(cameraPosition);
    Mat4 model = Mat4::identity();

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            cameraPosition.y += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            cameraPosition.y -= 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            cameraPosition.x -= 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            cameraPosition.x += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            cameraPosition.z += 0.05f;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            cameraPosition.z -= 0.05f;
        }

        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            rotationX += 0.015f;
        }
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            rotationX -= 0.015f;
        }

        continuousRotationAngle += 0.005f;
        Mat4 rotation = Mat4::rotate(continuousRotationAngle, Vec3(0.0f, 1.0f, 0.0f));

        model = Mat4::rotate(rotationX, Vec3(1.0f, 0.0f, 0.0f)) *
                Mat4::rotate(rotationY, Vec3(0.0f, 1.0f, 0.0f)) *
                rotation;

        view = Mat4::translate(cameraPosition);

        // Gestion des touches pour les shaders
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

        // Gestion de la transition avec la touche 7
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !transitioning) {
            transitioning = true;
            showingTexture = !showingTexture;
        }

        if (transitioning) {
            if (showingTexture) {
                mixFactor += 0.005f;
                if (mixFactor >= 1.0f) {
                    mixFactor = 1.0f;
                    transitioning = false;
                }
            } else {
                mixFactor -= 0.005f;
                if (mixFactor <= 0.0f) {
                    mixFactor = 0.0f;
                    transitioning = false;
                }
            }
            currentShaderProgram = transitionShaderProgram;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glUseProgram(currentShaderProgram);

        GLuint viewLoc = glGetUniformLocation(currentShaderProgram, "view");
        GLuint projLoc = glGetUniformLocation(currentShaderProgram, "projection");
        GLuint modelLoc = glGetUniformLocation(currentShaderProgram, "model");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.elements);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection.elements);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.elements);

        if (currentShaderProgram == transitionShaderProgram) {
            GLuint mixFactorLoc = glGetUniformLocation(currentShaderProgram, "mixFactor");
            glUniform1f(mixFactorLoc, mixFactor);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            GLuint textureLoc = glGetUniformLocation(currentShaderProgram, "ourTexture");
            glUniform1i(textureLoc, 0);
        }

		if (currentShaderProgram == phongShaderProgram) {
        GLint lightPosLoc = glGetUniformLocation(currentShaderProgram, "lightPos");
        GLint viewPosLoc = glGetUniformLocation(currentShaderProgram, "viewPos");
        GLint lightColorLoc = glGetUniformLocation(currentShaderProgram, "lightColor");
        GLint objectColorLoc = glGetUniformLocation(currentShaderProgram, "objectColor");

        glUniform3f(lightPosLoc, 1.2f, 1.0f, 2.0f);
        glUniform3f(viewPosLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
        glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
        glUniform3f(objectColorLoc, 1.0f, 0.5f, 0.31f);
    }
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    glDeleteBuffers(1, &colorVBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
