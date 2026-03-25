/**
 * @file    texture.cpp
 * @brief   Texture class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

#include <iostream>

#include <GL/glew.h>

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

    Texture::Texture(std::string name, int width, int height, const std::vector<float>& data)
        : Object(name),
          texId(0)
    {
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &texId);
    }

    void Texture::render(glm::mat4 modelview)
    {
        glBindTexture(GL_TEXTURE_2D, texId);
    }

    void Texture::updateSubImage(int offsetX, int offsetY, int width, int height, const std::vector<float>& data, int totalImageWidth)
    {
        // 1. Attiviamo questa texture
        glBindTexture(GL_TEXTURE_2D, texId);

        // 2. Diciamo a OpenGL quanto è "larga" l'immagine originale in memoria
        // Così sa di quanti float deve saltare per andare a capo-riga del quadratino
        glPixelStorei(GL_UNPACK_ROW_LENGTH, totalImageWidth);

        // 3. Calcoliamo il puntatore al primissimo pixel in alto a sinistra del nostro quadratino modificato.
        // Moltiplichiamo per 3 perché ogni pixel ha 3 float (R, G, B)
        const float* subImageStartPtr = &data[(offsetY * totalImageWidth + offsetX) * 3];

        // 4. Inviamo SOLO il quadratino alla GPU! Operazione quasi istantanea.
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            offsetX, offsetY,
            width, height,
            GL_RGB,
            GL_FLOAT,
            subImageStartPtr
        );

        // 5. IMPORTANTISSIMO: Ripristiniamo il comportamento standard di OpenGL!
        // Se non rimettiamo questo a 0, il caricamento delle prossime texture esploderà.
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(bitmap));
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}; // end of namespace Eng::
