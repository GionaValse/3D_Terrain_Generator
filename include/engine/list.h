/**
 * @file    list.h
 * @brief   List class
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#pragma once

/**
 * @brief Represents a sorted or categorized collection of scene objects for efficient rendering and processing.
 *
 * The List class is likely a core component of the rendering pipeline, used to manage visible
 * scene elements (meshes and lights) after a scene graph traversal (culling/sorting). It also
 * contains logic for specialized rendering effects, such as projection shadows.
 */
class ENG_API List : public Eng::Object
{
private:
    /**
     * @brief Structure representing a single instance of a scene object for the list.
     *
     * This holds the object pointer and its calculated world space transformation.
     */
    struct Instance
    {
        /** @brief Pointer to the scene graph node. */
        Eng::Node* node;
        /** @brief The node's final accumulated world coordinate matrix. */
        glm::mat4 nodeWorldMatrix;
    };

    /** @brief The general list of all scene objects considered for processing. */
    std::list<Instance> list;
    /** @brief A sub-list containing only light sources for easy access during shading. */
    std::list<Instance> lightList;
    /** @brief A sub-list containing only renderable meshes. */
    std::list<Instance> meshList;

    /** @brief A pointer to the currently active camera, needed for culling and view-dependent rendering. */
    Eng::Camera* camera;

    /**
     * @brief Creates a shadow projection matrix based on a light position and a planar surface.
     *
     * This is typically used for stencil buffer projection shadows (shadow volumes). 
     * @param lightPos The position of the light source in world space (homogeneous coordinates).
     * @param planeEquation The coefficients ($A, B, C, D$) of the plane onto which the shadow is cast.
     * @return The \c glm::mat4 shadow projection matrix.
     */
    glm::mat4 createShadowMatrix(const glm::vec4& lightPos, const glm::vec4& planeEquation);
    
    /**
     * @brief Renders shadows onto a specified plane using the calculated shadow matrices.
     *
     * This method iterates through the \c meshList and applies the shadow projection.
     * @param planeEquation The coefficients of the plane where shadows should be drawn.
     */
    void renderShadows(const glm::vec4& planeEquation);

    // Shaders
    Eng::Shader* mainShader = nullptr;
    int projLoc = -1;
    int mvLoc = -1;

public:
    void initShaders();
    /**
     * @brief Constructor for the List class.
     * @param name The name of the object list.
     */
    List(std::string name = "");

    /** @brief Virtual destructor. */
    virtual ~List();

    /**
     * @brief Executes the rendering loop for all objects currently in the list.
     *
     * This method typically iterates through the \c lightList (setting up light states)
     * and the \c meshList (drawing the geometry).
     * @param modelview The current modelview matrix (usually the camera's view matrix).
     */
    void render(glm::mat4 modelview = glm::mat4(1.0f)) override;

    /**
     * @brief Traverses the scene graph starting from a root node and populates the internal lists.
     *
     * This process typically involves calculating world matrices, culling non-visible objects,
     * and categorizing objects into \c lightList and \c meshList.
     * @param root The root node of the scene graph to begin traversal.
     * @param matrix The initial world matrix (usually identity).
     */
    void pass(Eng::Node* root, glm::mat4 matrix);

    /**
     * @brief Sets the active camera for the list, used for culling and view-dependent operations.
     * @param camera A pointer to the active \c Eng::Camera object.
     */
    void setCamera(Eng::Camera* camera);

    /**
     * @brief Clears all internal lists, removing all references to nodes and instances.
     */
    void clear();
};
