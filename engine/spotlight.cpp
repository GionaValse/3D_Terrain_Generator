/**
 * @file    spotlight.cpp
 * @brief   SpotLight class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

// Freeglut:
#include <GL/freeglut.h>

namespace Eng
{

    //////////////////
    // SpotLight CLASS //
    //////////////////

    SpotLight::SpotLight(std::string name, glm::mat4 matrix, float cutoff)
        : Eng::Light(name, matrix)
    {
        setCutoff(cutoff);
    }

    SpotLight::~SpotLight()
    {
    }

    void SpotLight::render(glm::mat4 modelview)
    {
        Light::render(modelview);

        glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        glm::vec4 position(0, 0, 0, 1.0f);
        glm::vec3 direction(0, -1, 0);

        glLightfv(GL_LIGHT0 + getLightID(), GL_POSITION, glm::value_ptr(position));
        glLightfv(GL_LIGHT0 + getLightID(), GL_SPOT_CUTOFF, &cutoff);
        glLightfv(GL_LIGHT0 + getLightID(), GL_SPOT_DIRECTION, glm::value_ptr(direction));

        glLoadMatrixf(glm::value_ptr(modelview * r));
        glm::vec4 emission(0.3f, 0.0f, 0.0f, 1.0f);

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(emission));
        // glutSolidCone(50, 70, 200, 200);
    }

    float SpotLight::getCutoff() const
    {
        return cutoff;
    }

    void SpotLight::setCutoff(float cutoff_)
    {
        cutoff = (cutoff_ < 0 || cutoff_ > 90) ? 0 : cutoff_;
    }

}; // end of namespace Eng::
