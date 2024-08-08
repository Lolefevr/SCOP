#include "obj_loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<unsigned int> indices;
std::vector<glm::vec2> texCoords;
std::vector<glm::vec3> colors;

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
        } else if (prefix == "vn") {
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        } else if (prefix == "vt") {
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            texCoords.push_back(texCoord);
        } else if (prefix == "f") {
            std::vector<unsigned int> vertexIndices, normalIndices, texCoordIndices;
            std::string vertexData;
            while (iss >> vertexData) {
                std::replace(vertexData.begin(), vertexData.end(), '/', ' ');
                std::istringstream viss(vertexData);
                unsigned int vertexIndex, normalIndex, texCoordIndex;
                viss >> vertexIndex;
                vertexIndices.push_back(vertexIndex - 1);
                if (viss.peek() == ' ') viss.ignore();
                if (viss >> texCoordIndex) {
                    texCoordIndices.push_back(texCoordIndex - 1);
                }
                if (viss.peek() == ' ') viss.ignore();
                if (viss >> normalIndex) {
                    normalIndices.push_back(normalIndex - 1);
                }
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
