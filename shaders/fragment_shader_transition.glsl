#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Color;
in vec2 TexCoords;

uniform sampler2D ourTexture;
uniform float mixFactor; // Facteur d'interpolation entre couleur et texture

void main() {
    vec4 textureColor = texture(ourTexture, TexCoords);
    vec4 vertexColor = vec4(Color, 1.0);

    // Interpolation entre la couleur des sommets et la texture
    FragColor = mix(vertexColor, textureColor, mixFactor);
}
