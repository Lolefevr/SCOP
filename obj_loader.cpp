#include "obj_loader.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

std::vector<float> vertices;
std::vector<float> normals;
std::vector<unsigned int> indices;
std::vector<float> texCoords;
std::vector<float> colors;

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
      float vertex[3];
      iss >> vertex[0] >> vertex[1] >> vertex[2];
      vertices.insert(vertices.end(), std::begin(vertex), std::end(vertex));

      float color[3] = {(rand() % 100) / 100.0f, (rand() % 100) / 100.0f,
                        (rand() % 100) / 100.0f};
      colors.insert(colors.end(), std::begin(color), std::end(color));
    } else if (prefix == "vn") {
      float normal[3];
      iss >> normal[0] >> normal[1] >> normal[2];
      normals.insert(normals.end(), std::begin(normal), std::end(normal));
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

        float v0[3] = {vertices[vertexIndices[0] * 3],
                       vertices[vertexIndices[0] * 3 + 1],
                       vertices[vertexIndices[0] * 3 + 2]};
        float v1[3] = {vertices[vertexIndices[i] * 3],
                       vertices[vertexIndices[i] * 3 + 1],
                       vertices[vertexIndices[i] * 3 + 2]};
        float v2[3] = {vertices[vertexIndices[i + 1] * 3],
                       vertices[vertexIndices[i + 1] * 3 + 1],
                       vertices[vertexIndices[i + 1] * 3 + 2]};

        float edgeLength1 =
            std::sqrt(std::pow(v1[0] - v0[0], 2) + std::pow(v1[1] - v0[1], 2) +
                      std::pow(v1[2] - v0[2], 2));
        float edgeLength2 =
            std::sqrt(std::pow(v2[0] - v1[0], 2) + std::pow(v2[1] - v1[1], 2) +
                      std::pow(v2[2] - v1[2], 2));
        float edgeLength3 =
            std::sqrt(std::pow(v2[0] - v0[0], 2) + std::pow(v2[1] - v0[1], 2) +
                      std::pow(v2[2] - v0[2], 2));

        float maxEdgeLength = std::max({edgeLength1, edgeLength2, edgeLength3});
        float uvScale = 1.0f / maxEdgeLength;

        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);
        texCoords.push_back(edgeLength1 * uvScale);
        texCoords.push_back(0.0f);
        texCoords.push_back(0.0f);
        texCoords.push_back(edgeLength2 * uvScale);
      }
    }
  }

  objFile.close();
}

void calculateCentroid(const std::vector<float>& vertices, float* centroid) {
  centroid[0] = centroid[1] = centroid[2] = 0.0f;
  for (size_t i = 0; i < vertices.size(); i += 3) {
    centroid[0] += vertices[i];
    centroid[1] += vertices[i + 1];
    centroid[2] += vertices[i + 2];
  }
  centroid[0] /= (vertices.size() / 3);
  centroid[1] /= (vertices.size() / 3);
  centroid[2] /= (vertices.size() / 3);
}

void centerVertices(std::vector<float>& vertices, const float* centroid) {
  for (size_t i = 0; i < vertices.size(); i += 3) {
    vertices[i] -= centroid[0];
    vertices[i + 1] -= centroid[1];
    vertices[i + 2] -= centroid[2];
  }
}
