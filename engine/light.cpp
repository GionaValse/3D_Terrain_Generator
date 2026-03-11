/**
 * @file    light.cpp
 * @brief   Light class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

// Freeglut:
#include <GL/freeglut.h>

#include <iostream>

int Eng::Light::ligthCounter = 0;

namespace Eng {

////////////////
// Light CLASS //
////////////////

Light::Light(std::string name, glm::mat4 matrix, glm::vec4 position)
    : Node(name, matrix),
      attenuation(1.0f, 0.0f, 0.0f),
      ambient(1.0f),
      diffuse(1.0f),
      specular(1.0f),
      position(position) {
    GLint maxLights = 0;
    glGetIntegerv(GL_MAX_LIGHTS, &maxLights);

    if (maxLights <= 0) maxLights = 6;

#if defined(DEBUG) || defined(_DEBUG)
    std::cout << "Max number of lights: " << maxLights << std::endl;
#endif

    if (ligthCounter < maxLights) {
        lightId = ligthCounter++;
        glEnable(GL_LIGHT0 + lightId);
        std::cout << "Enabled light: " << lightId << std::endl;
    }
    else {
        lightId = -1;
        std::cerr << "Warning: Max OpenGL lights reached!" << std::endl;
    }
}

Light::~Light() {
    if (lightId != -1) {
        ligthCounter--;
    }
}

void Light::render(glm::mat4 modelview) {
    if (lightId == -1) return;

    glLoadMatrixf(glm::value_ptr(modelview));

    glLightfv(GL_LIGHT0 + lightId, GL_AMBIENT, glm::value_ptr(ambient));
    glLightfv(GL_LIGHT0 + lightId, GL_DIFFUSE, glm::value_ptr(diffuse));
    glLightfv(GL_LIGHT0 + lightId, GL_SPECULAR, glm::value_ptr(specular));

    glLightf(GL_LIGHT0 + lightId, GL_CONSTANT_ATTENUATION, attenuation.x);   // default 1
    glLightf(GL_LIGHT0 + lightId, GL_LINEAR_ATTENUATION, attenuation.y);     // default 0
    glLightf(GL_LIGHT0 + lightId, GL_QUADRATIC_ATTENUATION, attenuation.z);  // default 0
}

int Light::getLightID() {
    return lightId;
}

glm::vec4 Light::getAmbient() {
    return ambient;
}

glm::vec4 Light::getDiffuse() {
    return diffuse;
}

glm::vec4 Light::getSpecular() {
    return specular;
}

void Light::setAmbient(glm::vec4 v) {
    ambient = v;
}

void Light::setDiffuse(glm::vec4 v) {
    diffuse = v;
}

void Light::setSpecular(glm::vec4 v) {
    specular = v;
}

void Light::setAttenuaton(float constant, float linear, float quadratic) {
    attenuation = glm::vec3(constant, linear, quadratic);
}

};  // namespace Eng
