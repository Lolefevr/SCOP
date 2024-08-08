#include "shaders.h"

const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform float mixValue;
uniform int state; // 0: wireframe, 1: colored faces, 2: textured faces

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    vec4 color = vec4(ourColor, 1.0);

    if (state == 0) {
        FragColor = vec4(ourColor, 1.0); // Fil de fer coloré
    } else if (state == 1) {
        FragColor = mix(vec4(ourColor, 1.0), texColor, mixValue); // Transition douce vers la texture
    } else {
        FragColor = mix(color, texColor, mixValue); // Faces texturées
    }
}
)glsl";
