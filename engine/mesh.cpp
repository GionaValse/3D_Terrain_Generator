/**
 * @file    mesh.cpp
 * @brief   Mesh class implementation
 *
 * @author  Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

#include "engine.h"

// GLEW:
#include <GL/glew.h>

namespace Eng
{

    /////////////////
    // MESH CLASS //
    /////////////////

    Mesh::Mesh(const std::string& name, const glm::mat4& matrix,
        std::vector<glm::vec3> vertexes,
        std::vector<glm::uvec3> faces,
        std::vector<glm::vec4> normals,
        std::vector<glm::vec2> textureCoordinates)
        : Node(name, matrix),
        vertexes{ vertexes },
        faces{ faces },
        normals{ normals },
        textureCoordinates{ textureCoordinates },
        buffersInitialized{ false },
        material{ nullptr }
    {
    }

    Mesh::~Mesh()
    {
        deleteBuffers();
    }

    void Mesh::render(glm::mat4 modelview)
    {
        //glLoadMatrixf(glm::value_ptr(modelview));
      
        if (vertexes.empty())
            return;

		if (!buffersInitialized) // Lazy initialization of OpenGL buffers when the mesh is first rendered
			initBuffers();

        if (material)
            material->render();

        // Rendering the mesh
		glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, size(faces) * 3, GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
    }

    void Mesh::setMaterial(Material* materialPtr)
    {
        this->material = materialPtr;
    }

    Material* Mesh::getMaterial()
    {
        return this->material;
    }

    void Mesh::initBuffers()
    {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

        if (!vertexes.empty())
        {
            glGenBuffers(1, &vboVertexes);
			glBindBuffer(GL_ARRAY_BUFFER, vboVertexes);
			glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(glm::vec3), vertexes.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(0);
        }

        if (!normals.empty())
        {
			glGenBuffers(1, &vboNormals);
			glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
			glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(1);
        }

        if (!textureCoordinates.empty())
        {
			glGenBuffers(1, &vboTextureCoordinates);
			glBindBuffer(GL_ARRAY_BUFFER, vboTextureCoordinates);
			glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(glm::vec2), textureCoordinates.data(), GL_STATIC_DRAW);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
			glEnableVertexAttribArray(2);
        }

        if (!faces.empty())
        {
			glGenBuffers(1, &vboFaces);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFaces);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(glm::uvec3), faces.data(), GL_STATIC_DRAW);
        }

		glBindVertexArray(0);

		buffersInitialized = true;
    }

    void Mesh::deleteBuffers()
    {
        if (vboVertexes) glDeleteBuffers(1, &vboVertexes);
        if (vboFaces) glDeleteBuffers(1, &vboFaces);
        if (vboNormals) glDeleteBuffers(1, &vboNormals);
        if (vboTextureCoordinates) glDeleteBuffers(1, &vboTextureCoordinates);
        if (vao) glDeleteVertexArrays(1, &vao);
    }
}; // end of namespace Eng::