/**
 * @file    omnilight.cpp
 * @brief   OmniLight class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

// Freeglut:
#include <GL/freeglut.h>

namespace Eng
{

    /////////////////////
    // OmniLight CLASS //
    /////////////////////

    OmniLight::OmniLight(std::string name, glm::mat4 matrix)
        : Light(name, matrix), cutoff(180.0f)
    {
    }

    OmniLight::~OmniLight()
    {
    }

    void OmniLight::render(glm::mat4 modelview)
    {

        glm::vec4 position(0, 0, 0, 1.0f);
        Light::render(modelview);

        glm::vec4 emission(1.0f, 1.0f, 1.0f, 1.0f);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(emission));
        glutSolidSphere(12, 32, 32);

        glLightfv(GL_LIGHT0 + getLightID(), GL_POSITION, glm::value_ptr(position));
        glLightfv(GL_LIGHT0 + getLightID(), GL_SPOT_CUTOFF, &cutoff);
    }

}; // end of namespace Eng::