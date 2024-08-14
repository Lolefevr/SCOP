#version 330 core

flat in int faceID;
out vec4 FragColor;

void main() {
    // Convertir l'identifiant de la face en une couleur de gris
    float grayValue = mod(float(faceID) * 0.1, 1.0); // 0.1 peut être ajusté pour changer la variation de gris
    FragColor = vec4(vec3(grayValue), 1.0);
}
