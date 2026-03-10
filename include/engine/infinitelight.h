/**
 * @file	infinitelight.h
 * @brief	InfiniteLight class header file
 *
 * @author	Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

 /**
  * @brief Implements an infinite or directional light source.
  *
  * An InfiniteLight simulates a light source that is infinitely far away (like the sun).
  * All light rays are parallel, and the light intensity does not diminish with distance.
  * This light type is defined only by its direction, which is stored in the inherited
  * Eng::Node transformation matrix.
  */
class ENG_API InfiniteLight : public Eng::Light
{
public:
    /**
     * @brief Constructor for the InfiniteLight.
     * @param name The name of the light.
     * @param matrix The initial transformation matrix, where the Z-axis defines the light's direction.
     */
    InfiniteLight(std::string name = "", glm::mat4 matrix = glm::mat4(1.0f));
    
    /** @brief Virtual destructor for the InfiniteLight. */
    virtual ~InfiniteLight();
    
    /**
     * @brief Deletes the attenuation setter from the base Light class.
     * @note Attenuation is not applicable to directional lights because they have infinite range.
     * @delete
     */
    void setAttenuaton(float constant = 1, float linear = 0, float quadratic = 0) = delete;

    /**
     * @brief Enables the light and sets its direction in the rendering pipeline.
     * @param modelview The accumulated Model-View matrix, used to transform the light's direction into view space.
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;
};
