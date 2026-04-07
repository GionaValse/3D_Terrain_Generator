#pragma once

class ISetupListener
{
public:
	virtual ~ISetupListener() = default;

	virtual void onTerrainGenerationRequested() = 0;
};