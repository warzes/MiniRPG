#pragma once

#include "GraphicsResources.h"
#include "MVector.h"

struct GraphicsSystemCreateInfo final
{
};

class GraphicsSystem
{
public:
	GraphicsSystem(Systems& systems);
	~GraphicsSystem();
	bool Create(const GraphicsSystemCreateInfo& createInfo);
	void Destroy();

	void Update();
	void Frame();

	void AddObject(const std::shared_ptr<IGraphicsObject>& object);

private:
	Systems& m_systems;
	MVector<std::shared_ptr<IGraphicsObject>> m_objects;
};