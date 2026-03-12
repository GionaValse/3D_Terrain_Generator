#version 440 core

layout (location = 0) in vec3 aPos;       // Posizione piatta dalla griglia
layout (location = 2) in vec2 aTexCoord;  // Coordinate UV

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightMap;
uniform float heightScale;
uniform float texelSize; // 1.0 / risoluzione_texture (es. 1.0/1024.0)

void main() {
    // 1. Lettura altezza attuale
    float h = texture(heightMap, aTexCoord).r * heightScale;
    
    // 2. Calcolo Normali via Central Differencing
    // Campioniamo i vicini (Destra, Sinistra, Sopra, Sotto)
    float hL = texture(heightMap, aTexCoord + vec2(-texelSize, 0.0)).r * heightScale;
    float hR = texture(heightMap, aTexCoord + vec2(texelSize, 0.0)).r * heightScale;
    float hD = texture(heightMap, aTexCoord + vec2(0.0, -texelSize)).r * heightScale;
    float hU = texture(heightMap, aTexCoord + vec2(0.0, texelSize)).r * heightScale;

    // Calcoliamo i vettori tangenti e la loro normale (prodotto vettoriale semplificato)
    vec3 n;
    n.x = hL - hR;
    n.z = hD - hU;
    n.y = 2.0 * texelSize * heightScale; // Questo bilancia l'intensità delle ombre
    
    Normal = normalize(mat3(transpose(inverse(model))) * n);

    // 3. Output posizione
    vec3 displacedPos = vec3(aPos.x, h, aPos.z);
    FragPos = vec3(model * vec4(displacedPos, 1.0));
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}