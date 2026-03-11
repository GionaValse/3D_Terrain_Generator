/**
 * @file	omnilight.h
 * @brief	OmniLight class header file
 *
 * @author	Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

 /**
  * @brief Implements an omnidirectional (point) light source.
  *
  * An OmniLight emits light equally in all directions from a specific point in space.
  * Its intensity typically diminishes over distance based on the attenuation factors
  * inherited from the Eng::Light base class. This is used to simulate light bulbs and small local sources.
  */
class ENG_API OmniLight : public Eng::Light
{
private:
    /**
     * @brief Cutoff value. Typically unused or set to 180 degrees for a true omnidirectional light source.
     * @note This member might be extraneous for an OmniLight, but is kept for consistency with the light base model.
     */
    float cutoff;

public:
    /**
     * @brief Constructor for the OmniLight.
     * @param name The name of the light.
     * @param matrix The initial transformation matrix, where the translation component defines the light's position.
     */
    OmniLight(std::string name = "", glm::mat4 matrix = glm::mat4(1.0f));
    
    /** @brief Virtual destructor for the OmniLight. */
    virtual ~OmniLight();

    /**
     * @brief Enables the light and sets its position and properties in the rendering pipeline.
     * @param modelview The accumulated Model-View matrix, used to transform the light's position into view space.
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;
};
