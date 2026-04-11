#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include <atomic>

#include "engine.h"

namespace fs = std::filesystem;

class ObjExporter
{
public:
    static void exportToObj(
        const std::string& fileName,
        const Eng::Mesh& mesh,
        const std::vector<float>& image,
        int size,
        float heightScale,
        std::atomic<float>* progress = nullptr)
    {
        fs::path filePath(fileName);
        fs::path dir = filePath.parent_path();

        if (!dir.empty() && !fs::exists(dir)) {
            try {
                fs::create_directories(dir);
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Error creating directory: " << e.what() << "\n";
                return;
            }
        }

        std::ofstream file(fileName);
        if (!file.is_open()) {
            std::cerr << "Unable to open the file in write mode.\n";
            return;
        }

        const auto& vertexes = mesh.getVertexes();
        const auto& texCoords = mesh.getTextureCoordinates();
        const auto& normals = mesh.getNormals();
        const auto& faces = mesh.getFaces();

        size_t totalOps = vertexes.size() + texCoords.size() * 2 + faces.size();
        size_t completedOps = 0;

        auto updateProgress = [&]()
            {
                completedOps++;
                if (progress && (completedOps % 1000 == 0 || completedOps == totalOps))
                    *progress = static_cast<float>(completedOps) / totalOps;
            };

        // --- START EXPORTING --- //
        file << "# Exported Terrain Mesh\n";

        auto getH = [&](float u, float v_coord)
            {
                int x = std::clamp(int(u * (size - 1)), 0, size - 1);
                int y = std::clamp(int(v_coord * (size - 1)), 0, size - 1);

                return image[(y * size + x) * 3] * heightScale;
            };

        file << "# Exported Terrain Mesh\n";

        // --- WRITING VERTEXES --- //
        for (size_t i = 0; i < vertexes.size(); ++i) 
        {
            float h = getH(texCoords[i].x, texCoords[i].y);
            file << "v " << vertexes[i].x << " " << h << " " << vertexes[i].z << "\n";
            updateProgress();
        }

        // --- WRITING UV COORDINATES --- //
        for (const auto& vt : texCoords)
        {
            file << "vt " << vt.x << " " << (1.0f - vt.y) << "\n";
            updateProgress();
        }

        // --- WRITING NORMALS --- //
        float texelSize = 1.0f / (float)size;
        for (const auto& vt : texCoords) 
        {
            float hL = getH(vt.x - texelSize, vt.y);
            float hR = getH(vt.x + texelSize, vt.y);
            float hD = getH(vt.x, vt.y - texelSize);
            float hU = getH(vt.x, vt.y + texelSize);

            float nx = hL - hR;
            float nz = hD - hU;
            float ny = 2.0f * texelSize * heightScale;

            float length = std::sqrt(nx * nx + ny * ny + nz * nz);
            file << "vn " << nx / length << " " << ny / length << " " << nz / length << "\n";

            updateProgress();
        }

        // --- WRITING FACES --- //
        for (const auto& face : faces) 
        {
            unsigned int i1 = face.x + 1;
            unsigned int i2 = face.y + 1;
            unsigned int i3 = face.z + 1;

            file << "f " << i1 << "/" << i1 << "/" << i1 << " "
                << i2 << "/" << i2 << "/" << i2 << " "
                << i3 << "/" << i3 << "/" << i3 << "\n";

            updateProgress();
        }

        file.close();
        std::cout << "OBJ file saved successfully to: " << fileName << "\n";
    }
};
