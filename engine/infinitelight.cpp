/**
 * @file    infinitelight.cpp
 * @brief   InfiniteLight class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"


 // Freeglut:
#include <GL/freeglut.h>

namespace Eng
{

    /////////////////////////
    // InfiniteLight CLASS //
    /////////////////////////

    InfiniteLight::InfiniteLight(std::string name, glm::mat4 matrix)
        : Light(name, matrix)
    {
    }

    InfiniteLight::~InfiniteLight()
    {
    }

      void InfiniteLight::render(glm::mat4 modelview)
    {
        Light::render(modelview);

        glm::vec4 position(0.0f);
        glLightfv(GL_LIGHT0 + getLightID(), GL_POSITION, glm::value_ptr(position));
    }

}; // end of namespace Eng::