/**
 * @file	spotlight.h
 * @brief	SpotLight class header file
 *
 * @author	Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

/**
 * @brief Implements a light source that emits light in a focused cone shape.
 *
 * A SpotLight is defined by its position, direction, and a cutoff angle, which determines the
 * size of the cone of light. It inherits attenuation properties from Eng::Light.
 * 
 */
class ENG_API SpotLight : public Eng::Light
{
private:
    /** @brief The angle (in degrees or radians) defining the width of the light cone. Light is emitted only within this angle. */
    float cutoff;

public:
    /**
     * @brief Constructor for the SpotLight.
     * @param name The name of the light.
     * @param matrix The initial transformation matrix, where the position is the light origin and the Z-axis defines the direction.
     * @param cutoff The initial cutoff angle for the light cone (default 45 degrees).
     */
    SpotLight(std::string name = "", glm::mat4 matrix = glm::mat4(1.0f), float cutoff = 45);
    
    /** @brief Virtual destructor for the SpotLight. */
    virtual ~SpotLight();

    /**
     * @brief Enables the light and sets its position, direction, and cutoff angle in the rendering pipeline.
     * @param modelview The accumulated Model-View matrix.
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;

    /**
     * @brief Retrieves the current cutoff angle of the light cone.
     * @return The cutoff angle in the defined units (degrees or radians).
     */
    float getCutoff() const;

    /**
     * @brief Sets the cutoff angle for the light cone.
     *
     * The input is clamped between 0 and 90 degrees to ensure a valid light cone shape.
     * @param cutoff_ The new cutoff angle to set.
     */
    void setCutoff(float cutoff_);
};
