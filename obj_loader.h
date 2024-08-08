#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

extern std::vector<glm::vec3> vertices;
extern std::vector<glm::vec3> normals;
extern std::vector<unsigned int> indices;
extern std::vector<glm::vec2> texCoords;
extern std::vector<glm::vec3> colors;

void loadOBJ(const std::string& path);
glm::vec3 calculateCentroid(const std::vector<glm::vec3>& vertices);
void centerVertices(std::vector<glm::vec3>& vertices, const glm::vec3& centroid);

#endif // OBJ_LOADER_H
