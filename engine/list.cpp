/**
 * @file    list.cpp
 * @brief   List class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

 // Freeglut:
#include <GL/freeglut.h>

// GLM:
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp> // Necessario per glm::translate

#include <iostream>

namespace Eng
{

    ////////////////
    // List CLASS //
    ////////////////

    ENG_API List::List(std::string name)
        : Object(name),
        camera(nullptr)
    {
    }

    List::~List()
    {
    }

    glm::mat4 List::createShadowMatrix(const glm::vec4& lightPos, const glm::vec4& planeEquation)
    {
        glm::mat4 shadowMat;
        float dot = glm::dot(planeEquation, lightPos);

        shadowMat[0][0] = dot - lightPos.x * planeEquation.x;
        shadowMat[1][0] = -lightPos.x * planeEquation.y;
        shadowMat[2][0] = -lightPos.x * planeEquation.z;
        shadowMat[3][0] = -lightPos.x * planeEquation.w;

        shadowMat[0][1] = -lightPos.y * planeEquation.x;
        shadowMat[1][1] = dot - lightPos.y * planeEquation.y;
        shadowMat[2][1] = -lightPos.y * planeEquation.z;
        shadowMat[3][1] = -lightPos.y * planeEquation.w;

        shadowMat[0][2] = -lightPos.z * planeEquation.x;
        shadowMat[1][2] = -lightPos.z * planeEquation.y;
        shadowMat[2][2] = dot - lightPos.z * planeEquation.z;
        shadowMat[3][2] = -lightPos.z * planeEquation.w;

        shadowMat[0][3] = -lightPos.w * planeEquation.x;
        shadowMat[1][3] = -lightPos.w * planeEquation.y;
        shadowMat[2][3] = -lightPos.w * planeEquation.z;
        shadowMat[3][3] = dot - lightPos.w * planeEquation.w;

        return shadowMat;
    }

    void List::renderShadows(const glm::vec4& planeEquation)
    {
        if (lightList.empty() || meshList.empty())
            return;

        Instance lightInst = lightList.front();
        glm::vec4 lightPos = glm::vec4(lightInst.nodeWorldMatrix[3]);

        glm::mat4 shadowMatrix = createShadowMatrix(lightPos, planeEquation);

        // Disabilitiamo luci e texture, usiamo un colore solido scuro
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.2f, 0.2f, 0.2f); // Grigio scuro opaco

        glm::mat4 viewMatrix = camera->getViewMatrix();

        Eng::Base::getInstance().setShadowRender(true);

        for (auto& inst : meshList)
        {
            glm::mat4 modelViewShadow = viewMatrix * shadowMatrix * inst.nodeWorldMatrix;
            // Solleviamo l'ombra di 0.01f per evitare z-fighting col pavimento
            modelViewShadow = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 0.0f)) * modelViewShadow;

            glPushMatrix();
            glLoadMatrixf(glm::value_ptr(modelViewShadow));

            inst.node->render(modelViewShadow);

            glPopMatrix();
        }

        Eng::Base::getInstance().setShadowRender(false);

        // Riabilitiamo solo l'illuminazione (le texture verranno gestite dalle mesh)
        glEnable(GL_LIGHTING);
    }

    void List::render(glm::mat4 modelview)
    {
        if (camera == nullptr)
        {
            std::cerr << "No camera found" << std::endl;
            return;
        }

        if (mainShader == nullptr)
            return;

        glMatrixMode(GL_MODELVIEW);
        glm::mat4 viewMatrix = camera->getViewMatrix();

        glm::vec4 groundPlane(0.0f, 1.0f, 0.0f, 0.0f);
        //renderShadows(groundPlane);

        mainShader->render();
        mainShader->setMatrix(projLoc, camera->getProjectionMatrix());

        for (auto inst : list)
        {
            glm::mat4 modelViewModel = viewMatrix * inst.nodeWorldMatrix;
            mainShader->setMatrix(mvLoc, modelViewModel);
            inst.node->render(modelViewModel);
        }
    }

    void List::pass(Node* root, glm::mat4 matrix)
    {
        matrix = matrix * root->getMatrix();

        Instance inst;
        inst.node = root;
        inst.node->calculateMove();
        inst.nodeWorldMatrix = root->getWorldCoordinateMatrix();

        if (dynamic_cast<Light*>(inst.node) != nullptr)
        {
            this->list.push_front(inst);
            this->lightList.push_back(inst);
        }
        else if (dynamic_cast<Mesh*>(inst.node) != nullptr)
        {
            this->list.push_back(inst);
            this->meshList.push_back(inst);
        }

        for (Node* child : root->getChildren())
        {
            this->pass(child, matrix);
        }
    }

    void List::setCamera(Eng::Camera* camera)
    {
        if (camera == nullptr)
        {
            std::cerr << "Cannot set a null camera" << std::endl;
            return;
        }

        this->camera = camera;

        if (mainShader)
            mainShader->setMatrix(projLoc, camera->getProjectionMatrix());
    }

    void List::clear()
    {
        list.clear();
        meshList.clear();
        lightList.clear();
    }

    // Shader

    void List::initShaders() {
        const char* vertShader = R"(
            #version 440 core
            layout(location = 0) in vec3 in_Position;
            uniform mat4 projection;
            uniform mat4 modelview;
            void main() {
                gl_Position = projection * modelview * vec4(in_Position, 1.0);
            }
        )";

        const char* fragShader = R"(
            #version 440 core
            out vec4 frag_Output;
            void main() {
                frag_Output = vec4(1.0, 1.0, 0.0, 1.0); // GIALLO
            }
        )";

        Eng::Shader* vs = new Shader();
        vs->loadFromMemory(Shader::TYPE_VERTEX, vertShader);

        Eng::Shader* fs = new Shader();
        fs->loadFromMemory(Shader::TYPE_FRAGMENT, fragShader);

        mainShader = new Shader();
        mainShader->build(vs, fs);

        projLoc = mainShader->getParamLocation("projection");
        mvLoc = mainShader->getParamLocation("modelview");
    }

}; // end of namespace Eng::