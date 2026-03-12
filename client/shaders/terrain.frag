#version 440 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;      // Posizione della luce (es. il sole)
uniform vec3 viewPos;       // Posizione della camera
uniform vec3 brushPos;      // Punto dove il mouse sta toccando (World Space)
uniform float brushRadius;  // Raggio del pennello per l'anteprima

void main() {
    // --- Illuminazione Base (Phong semplificato) ---
    vec3 ambient = 0.2 * vec3(1.0); // Luce ambientale
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(Normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(0.8);

    // --- Colore Dinamico ---
    // Esempio: Grigio se alto (montagna), Verde se basso (prato)
    vec3 terrainColor = mix(vec3(0.2, 0.5, 0.2), vec3(0.5, 0.5, 0.5), smoothstep(0.0, 10.0, FragPos.y));

    // --- Anteprima Pennello ---
    // Disegniamo un cerchio rosso dove l'utente sta per scolpire
    float distToBrush = distance(FragPos.xz, brushPos.xz);
    if (distToBrush < brushRadius && distToBrush > brushRadius - 0.1) {
        terrainColor = vec3(1.0, 0.0, 0.0); // Bordo del pennello rosso
    }

    vec3 result = (ambient + diffuse) * terrainColor;
    FragColor = vec4(result, 1.0);
}