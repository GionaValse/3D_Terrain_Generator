#version 440 core
out vec4 FragColor;

in vec3 FragPos;  // Dalla vertex shader
in vec3 Normal;   // Dalla vertex shader (calcolata dalla heightmap)
in vec2 TexCoord; // Dalla vertex shader

// --- Uniform del MATERIALE ---
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

// --- Uniform della LUCE (le tue) ---
uniform vec3 lightPosition;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

uniform vec3 viewPos; // Posizione della camera per i riflessi speculari

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);

    // 1. Ambient: Luce costante di base
    vec3 ambient = lightAmbient * matAmbient;

    // 2. Diffuse: Basata sull'inclinazione della normale rispetto alla luce
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * (diff * matDiffuse);

    // 3. Specular: Riflesso "lucido" (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), matShininess);
    vec3 specular = lightSpecular * (spec * matSpecular);

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}