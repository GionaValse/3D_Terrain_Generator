/**
 * @file    material.cpp
 * @brief   Material class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

// FreeGlut:
#include <GL/freeglut.h>

namespace Eng
{

    ////////////////
    // Material CLASS //
    ////////////////

    Material::Material(std::string name, glm::vec4 emission, glm::vec4 ambient, glm::vec4 diffuse, glm ::vec4 specular, float shiness)
        : Object(name),
          emission(emission),
          ambient(ambient),
          diffuse(diffuse),
          specular(specular),
          shininess(shiness),
          texture(nullptr)
    {
    }

    Material::~Material()
    {
        delete texture;
    }

    void Material::render(glm::mat4 modelview)
    {
        if (texture)
        {
            if (!Eng::Base::getInstance().getShadowRender())
            {
                glEnable(GL_TEXTURE_2D);
            }
            texture->render(modelview);
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }

        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(emission));
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(ambient));
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(diffuse));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(specular));

        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }

    glm::vec4 Material::getEmission() const
    {
        return emission;
    }

    glm::vec4 Material::getAmbient() const
    {
        return ambient;
    }

    glm::vec4 Material::getDiffuse() const
    {
        return diffuse;
    }

    glm::vec4 Material::getSpecular() const
    {
        return specular;
    }

    float Material::getShininess() const
    {
        return shininess;
    }

    Eng::Texture* Material::getTexture() const
    {
        return texture;
    }

    void Material::setEmission(const glm::vec4& emission_)
    {
        emission = emission_;
    }

    void Material::setAmbient(const glm::vec4& ambient_)
    {
        ambient = ambient_;
    }

    void Material::setDiffuse(const glm::vec4& diffuse_)
    {
        diffuse = diffuse_;
    }

    void Material::setSpecular(const glm::vec4& specular_)
    {
        specular = specular_;
    }

    void Material::setShininess(float shininess_)
    {
        shininess = shininess_;
    }

    void Material::setTexture(Eng::Texture* texture_)
    {
        texture = texture_;
    }

}; // end of namespace Eng::
