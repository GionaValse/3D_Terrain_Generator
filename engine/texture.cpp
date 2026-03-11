/**
 * @file    texture.cpp
 * @brief   Texture class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

#include <iostream>

// FreeGlut
#include <GL/freeglut.h>

// FreeImage
#include <FreeImage.h>

namespace Eng
{

    ////////////////
    // Texture CLASS //
    ////////////////

    Texture::Texture(std::string name, const std::string &filePath)
        : Object(name),
          texId(0)
    {
        if (!filePath.empty())
        {
            loadTexture(filePath);
        }
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &texId);
    }

    void Texture::render(glm::mat4 modelview)
    {
        glBindTexture(GL_TEXTURE_2D, texId);
    }

    void Texture::loadTexture(const std::string &filePath)
    {
        // Load an image from file:
        FIBITMAP *bitmap = FreeImage_Load(FreeImage_GetFileType(filePath.c_str(), 0), filePath.c_str());

        if (!bitmap)
        {
            std::cerr << "Unable to load texture " << getName() << std::endl;
            return;
        }

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        int width = FreeImage_GetWidth(bitmap);
        int height = FreeImage_GetHeight(bitmap);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(bitmap));
    }
}; // end of namespace Eng::
