#version 330 core

out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;

void main() {
    float intensity = dot(normalize(Normal), vec3(0.0, 0.0, 1.0));
    intensity = clamp(intensity, 0.0, 1.0);
    FragColor = vec4(vec3(intensity), 1.0);
}
