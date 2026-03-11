#pragma once
#include "TerrainConfig.hpp"
#include <vector>
#include <string>

namespace terrain
{

    class ImageExporter
    {
    public:
        static bool saveEXR(const std::vector<float> &image, const TerrainConfig &config, const std::string &outputDir = "bin/noises");
    };

}