/**
 * @file    texture.h
 * @brief   Texture class
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

/**
 * @brief Manages loading, storing, and applying image data as a texture map.
 *
 * The Texture class handles the process of loading an image file, uploading it to the
 * graphics processing unit (GPU) memory, and providing a handle (\c texId) for use
 * during the rendering pipeline. 
 */
class ENG_API Texture : public Eng::Object
{
public:
    /**
     * @brief Constructor for the Texture class.
     *
     * Initializes the texture object and immediately attempts to load the image from the specified path.
     * @param name The name of the texture object.
     * @param filePath The file system path to the image file (e.g., "assets/image.png").
     */
    Texture(std::string name = "", const std::string& filePath = "");

    /**
     * @brief Virtual destructor for the Texture class.
     *
     * Responsible for releasing the GPU resource associated with the texture ID.
     */
    virtual ~Texture();

    /**
     * @brief Binds the texture object to the active texture unit in the rendering pipeline.
     *
     * This makes the texture available for sampling by the current shader/material.
     * @param modelview This parameter is usually ignored for texture operations.
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;

private:
    /** @brief The unique identifier (handle) used by the graphics API (e.g., OpenGL texture ID) for the texture data on the GPU. */
    unsigned int texId;

    /**
     * @brief Internal method to load the image data from a file and upload it to the GPU.
     * @param filePath The file system path to the image file.
     */
    void loadTexture(const std::string& filePath);
};
