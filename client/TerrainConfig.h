#pragma once
#include <cstdint>


struct TerrainConfig
{
	int size = 512;


	bool isValid() const
	{
		return size > 0;
	}
};
