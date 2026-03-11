#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "engine.h"

namespace terrain {

    // Struttura del nostro vertice
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoords;
    };

    class GridGenerator {
    public:
        static Eng::Mesh* generate(int resolution) {
            std::vector<glm::vec3> vertexes = std::vector<glm::vec3>();
            std::vector<glm::uvec3> faces = std::vector<glm::uvec3>();
            std::vector<glm::vec4> normals = std::vector<glm::vec4>();
            std::vector<glm::vec2> textureCoordinates = std::vector<glm::vec2>();

            for (int z = 0; z <= resolution; ++z) {
                for (int x = 0; x <= resolution; ++x) {
                    //formula per far partire la gen dal centro (0,0,0)
                    float xPos = ((float)x / resolution) * 2.0f - 1.0f;
                    float zPos = ((float)z / resolution) * 2.0f - 1.0f;

                    glm::vec3 vertex = glm::vec3(xPos, 0.0f, zPos);
                    glm::vec2 textureCoord = glm::vec2((float)x / resolution, (float)z / resolution);

                    vertexes.push_back(vertex);
                    textureCoordinates.push_back(textureCoord);
                }
            }

            for (int z = 0; z < resolution; ++z) {
                for (int x = 0; x < resolution; ++x) {
                    unsigned int topLeft = z * (resolution + 1) + x;
                    unsigned int topRight = topLeft + 1;
                    unsigned int bottomLeft = (z + 1) * (resolution + 1) + x;
                    unsigned int bottomRight = bottomLeft + 1;

                    glm::uvec3 face1 = glm::uvec3(topLeft, bottomLeft, topRight);
                    glm::uvec3 face2 = glm::uvec3(topRight, bottomLeft, bottomRight);

                    glm::vec4 normal1 = glm::vec4(glm::cross(vertexes[topLeft], vertexes[bottomLeft]), 1.0f);
                    glm::vec4 normal2 = glm::vec4(glm::cross(vertexes[topRight], vertexes[bottomLeft]), 1.0f);

                    faces.push_back(face1);
                    faces.push_back(face2);

                    normals.push_back(normal1);
                    normals.push_back(normal2);
                }
            }

            Eng::Mesh* gridMesh = new Eng::Mesh(
                "GridMesh",
                glm::mat4(1.0f),
                vertexes,
                faces,
                normals,
                textureCoordinates
            );
            return gridMesh;
        }
    };
}