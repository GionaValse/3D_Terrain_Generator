#pragma once
#include "TerrainConfig.hpp"
#include <vector>
#include <string>

namespace terrain
{

    class ImageExporter
    {
    public:
        static bool saveEXR(
            const std::vector<float> &image, 
            const TerrainConfig &config, 
            std::string &fileName,
            const std::string &outputDir = "bin/noises"
        );
    };

}