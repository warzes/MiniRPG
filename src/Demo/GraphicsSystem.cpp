#include "stdafx.h"
#include "GraphicsSystem.h"

GraphicsSystem::GraphicsSystem(Systems& systems)
	: m_systems(systems)
{
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::Create(const GraphicsSystemCreateInfo& createInfo)
{
	return true;
}

void GraphicsSystem::Destroy()
{
}
