/**
 * @file    light.h
 * @brief   light class
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

/**
 * @brief Abstract base class for all light sources in the scene.
 *
 * The Light class inherits from Eng::Node, allowing it to be placed and oriented
 * within the scene graph. It holds the fundamental lighting properties: color components
 * (ambient, diffuse, specular), attenuation factors, and a unique OpenGL-style ID.
 * Concrete types (like OmniLight, SpotLight) derive from this class. 
 */
class ENG_API Light : public Eng::Node
{
    // IMPORTANT prof require this
    /**
     * @brief Static counter used to assign unique and incremental light IDs.
     * @note This is likely required to manage OpenGL's fixed function pipeline light limit (GL_LIGHT0, GL_LIGHT1, etc.).
     */
    static int ligthCounter;

    /** @brief The unique ID (e.g., 0, 1, 2) assigned to this light instance, corresponding to a hardware light slot. */
    int lightId;
    
    /** @brief Vector containing the constant, linear, and quadratic attenuation factors (\c x, \c y, \c z components) for distance falloff. */
    glm::vec3 attenuation;
    /** @brief The ambient color component of the light (\c R, \c G, \c B, \c A). This color is cast uniformly. */
    glm::vec4 ambient;
    /** @brief The diffuse color component of the light (\c R, \c G, \c B, \c A). This color provides direction-dependent illumination. */
    glm::vec4 diffuse;
    /** @brief The specular color component of the light (\c R, \c G, \c B, \c A). This color contributes to highlights. */
    glm::vec4 specular;
    /** @brief The light source's position in homogeneous coordinates (\c x, \c y, \c z, \c w). W=1 for point lights, W=0 for directional lights. */
    glm::vec4 position;

public:
    /**
     * @brief Constructor for the Light base class.
     *
     * Assigns a unique ID and initializes common properties.
     * @param name The name of the light (passed to Node).
     * @param matrix The initial local transformation matrix (passed to Node, default is identity).
     * @param position The position vector of the light source (default is (0, 0, 0, 0)).
     */
    Light(std::string name = "", glm::mat4 matrix = glm::mat4(1.0f), glm::vec4 position = glm::vec4(0.0f));
    
    /** @brief Virtual destructor for the Light base class. */
    virtual ~Light();

    /**
     * @brief Renders the light, which primarily involves enabling and setting its properties in the rendering pipeline (e.g., OpenGL state).
     * @param modelview The accumulated Model-View matrix, used to transform the light's position into view space.
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;

    /////////////
    // Getters //
    /////////////

    /**
     * @brief Retrieves the unique light ID assigned to this instance.
     * @return The light's integer ID.
     */
    int getLightID();

    /**
     * @brief Retrieves the current ambient color component.
     * @return The \c glm::vec4 ambient color.
     */
    glm::vec4 getAmbient();

    /**
     * @brief Retrieves the current diffuse color component.
     * @return The \c glm::vec4 diffuse color.
     */
    glm::vec4 getDiffuse();

    /**
     * @brief Retrieves the current specular color component.
     * @return The \c glm::vec4 specular color.
     */
    glm::vec4 getSpecular();

    /////////////
    // Setters //
    /////////////

    /**
     * @brief Sets the ambient color component of the light.
     * @param v The new \c glm::vec4 ambient color (default is (1.0f, 1.0f, 1.0f, 1.0f)).
     */
    void setAmbient(glm::vec4 v = glm::vec4(1.0f));

    /**
     * @brief Sets the diffuse color component of the light.
     * @param v The new \c glm::vec4 diffuse color (default is (1.0f, 1.0f, 1.0f, 1.0f)).
     */
    void setDiffuse(glm::vec4 v = glm::vec4(1.0f));

    /**
     * @brief Sets the specular color component of the light.
     * @param v The new \c glm::vec4 specular color (default is (1.0f, 1.0f, 1.0f, 1.0f)).
     */
    void setSpecular(glm::vec4 v = glm::vec4(1.0f));

    /**
     * @brief Sets the attenuation factors for the light, controlling how light intensity diminishes with distance.
     * @param constant The constant attenuation factor (default 1).
     * @param linear The linear attenuation factor (default 0).
     * @param quadratic The quadratic attenuation factor (default 0).
     */
    void setAttenuaton(float constant = 1, float linear = 0, float quadratic = 0);
};