#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out int faceID;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // Calcul d'un identifiant de face simple basé sur l'index du sommet
    faceID = gl_VertexID / 3;
}
