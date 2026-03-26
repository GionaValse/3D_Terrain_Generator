#version 440 core
out vec4 fragOutput;

in vec4 fragPosition;
in vec3 normal;
in vec2 texCoord;
in float v_Height;

// Materiale (stessi nomi dell'engine)
uniform vec3 matEmission;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;

// Luci (formato array dell'engine)
uniform vec3 lightPosition[8];
uniform vec3 lightDirection[8];
uniform vec3 lightAmbient[8];
uniform vec3 lightDiffuse[8];
uniform vec3 lightSpecular[8];
uniform int  lightType[8];
uniform float spotCutoffCos[8];
uniform float spotExponent[8];
uniform int activeLightCount;

// Texture
layout(binding = 1) uniform sampler2D texSampler;
uniform bool hasTexture;

// Terrain color
uniform bool hasColor;

void main() {
    vec4 texel = hasTexture ? texture(texSampler, texCoord) : vec4(1.0);

    vec3 terrainBaseColor = vec3(1.0f);

    if (hasColor) {
        vec3 colorGrass = vec3(0.2, 0.5, 0.2); // Verde
        vec3 colorRock  = vec3(0.5, 0.5, 0.5); // Grigio
        vec3 colorSnow  = vec3(0.9, 0.9, 0.9); // Bianco

        float rockBlend = smoothstep(0.3, 0.5, v_Height); 
        float snowBlend = smoothstep(0.7, 0.85, v_Height);

        terrainBaseColor = mix(colorGrass, colorRock, rockBlend);
        terrainBaseColor = mix(terrainBaseColor, colorSnow, snowBlend);
    }

    vec3 norm    = normalize(normal);
    vec3 viewDir = normalize(-fragPosition.xyz); // view space: camera all'origine

    vec3 totalLightColor = matEmission;
    vec3 specularLight = vec3(0.0);

    for (int i = 0; i < activeLightCount; i++) {
        vec3 lightDir;
        float attenuation = 1.0;

        if (lightType[i] == 1) {
            lightDir = normalize(-lightDirection[i]);
        } else {
            lightDir = normalize(lightPosition[i] - fragPosition.xyz);
        }

        if (lightType[i] == 2) {
            float spotCos = dot(normalize(lightDirection[i]), -lightDir);
            attenuation = (spotCos < spotCutoffCos[i]) ? 0.0 : pow(spotCos, spotExponent[i]);
        }

        if (lightType[i] == 0)
            attenuation = 3.0;

        if (attenuation > 0.0) {
            vec3 ambient  = matAmbient * lightAmbient[i];
            float nDotL   = max(dot(norm, lightDir), 0.0);
            vec3 diffuse  = matDiffuse * nDotL * lightDiffuse[i];

            vec3 specular = vec3(0.0);
            if (nDotL > 0.0) {
                vec3 halfVec = normalize(lightDir + viewDir);
                float nDotHV = max(dot(norm, halfVec), 0.0);
                specular = matSpecular * pow(nDotHV, matShininess) * lightSpecular[i];
            }

            totalLightColor += (ambient + diffuse + specular) * attenuation;
            specularLight += specular * attenuation;
        }
    }

    vec3 finalColor = (terrainBaseColor * texel.rgb * totalLightColor) + specularLight;
    fragOutput = vec4(finalColor, 1.0);
}