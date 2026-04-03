#pragma once

#include "BaseTool.h"

class IToolListener
{
public:
    virtual ~IToolListener() = default;
	virtual BaseTool* getActiveTool() const = 0;
    virtual void onToolSelected(BaseTool* tool) = 0;
};
