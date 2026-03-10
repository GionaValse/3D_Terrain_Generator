#pragma once
#include "../include/glm/glm.hpp"
#include <vector>

namespace terrain {

    // Struttura del nostro vertice
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoords;
    };

    class GridGenerator {
    public:
        static void generate(int resolution, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
            vertices.clear();
            indices.clear();

            for (int z = 0; z <= resolution; ++z) {
                for (int x = 0; x <= resolution; ++x) {
                    Vertex v;
                    //formula per far partire la gen dal centro (0,0,0)
                    float xPos = ((float)x / resolution) * 2.0f - 1.0f;
                    float zPos = ((float)z / resolution) * 2.0f - 1.0f;
                    
                    v.position = glm::vec3(xPos, 0.0f, zPos);
                    v.texCoords = glm::vec2((float)x / resolution, (float)z / resolution);
                    
                    vertices.push_back(v);
                }
            }

            for (int z = 0; z < resolution; ++z) {
                for (int x = 0; x < resolution; ++x) {
                    unsigned int topLeft     = z * (resolution + 1) + x;
                    unsigned int topRight    = topLeft + 1;
                    unsigned int bottomLeft  = (z + 1) * (resolution + 1) + x;
                    unsigned int bottomRight = bottomLeft + 1;

                    indices.push_back(topLeft);
                    indices.push_back(bottomLeft);
                    indices.push_back(topRight);

                    indices.push_back(topRight);
                    indices.push_back(bottomLeft);
                    indices.push_back(bottomRight);
                }
            }
        }
    };

}