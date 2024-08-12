#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <string>
#include <vector>

extern std::vector<float> vertices;
extern std::vector<float> normals;
extern std::vector<unsigned int> indices;
extern std::vector<float> texCoords;
extern std::vector<float> colors;

void loadOBJ(const std::string& path);
void calculateCentroid(const std::vector<float>& vertices, float* centroid);
void centerVertices(std::vector<float>& vertices, const float* centroid);

#endif  // OBJ_LOADER_H
