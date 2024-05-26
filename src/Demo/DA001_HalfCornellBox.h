#pragma once

#include "GraphicsResources.h"

class DA001_HalfCornellBox final : public IGraphicsObject
{
public:
	DA001_HalfCornellBox(Systems& systems, const std::string& gameObjectName, int executionOrder);
	void Init() final;
	void Update() final {}
};