#pragma once
#include <cstdint>

namespace terrain
{

    struct TerrainConfig
    {
        int size = 512;
        double frequency = 4.0;
        int octaves = 4;
        uint32_t seed = 123456;

        bool isValid() const
        {
            return size > 0 && octaves > 0 && frequency > 0;
        }
    };

}