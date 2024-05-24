#pragma once

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

private:
	Systems& m_systems;
};