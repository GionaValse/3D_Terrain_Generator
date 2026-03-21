#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "engine.h"

namespace terrain {

    class GridGenerator {
    public:
        static Eng::Mesh* generate(unsigned int size, float resolution = 1.0f) {
            std::vector<glm::vec3> vertexes;
            std::vector<glm::uvec3> faces;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec2> textureCoordinates;

            int segments = static_cast<int>(size / resolution);
            int vertexCount = segments + 1;

            vertexes.reserve(vertexCount * vertexCount);
            faces.reserve(segments * segments * 2);

            for (int z = 0; z < vertexCount; ++z)
            {
                for (int x = 0; x < vertexCount; ++x)
                {
                    float xPos = x * resolution - (size / 2.0f);
                    float zPos = z * resolution - (size / 2.0f);

                    vertexes.push_back(glm::vec3(xPos, 0.0f, zPos));
                    // normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)); // GPU edit normals by shaders

                    float xCoord = (float)x / segments;
                    float zCoord = (float)z / segments;

                    textureCoordinates.push_back(glm::vec2(xCoord, zCoord));
                }
            }

            for (int z = 0; z < segments; ++z)
            {
                for (int x = 0; x < segments; ++x)
                {
                    unsigned int topLeft = z * vertexCount + x;
                    unsigned int topRight = z * vertexCount + (x + 1);
                    unsigned int bottomLeft = (z + 1) * vertexCount + x;
                    unsigned int bottomRight = (z + 1) * vertexCount + (x + 1);

                    faces.push_back(glm::uvec3(topLeft, bottomLeft, topRight));
                    faces.push_back(glm::uvec3(topRight, bottomLeft, bottomRight));
                }
            }

            return new Eng::Mesh("GridMesh", glm::mat4(1.0f), vertexes, faces, normals, textureCoordinates);
        }
    };
}