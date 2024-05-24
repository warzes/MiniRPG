#include "stdafx.h"
#include "GraphicsSystem.h"
//-----------------------------------------------------------------------------
GraphicsSystem::GraphicsSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.graphics = this;
}
//-----------------------------------------------------------------------------
GraphicsSystem::~GraphicsSystem()
{
}
//-----------------------------------------------------------------------------
bool GraphicsSystem::Create(const GraphicsSystemCreateInfo& createInfo)
{
	return true;
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Destroy()
{
	m_objects.clear();
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Update()
{
}
//-----------------------------------------------------------------------------
void GraphicsSystem::Frame()
{
}
//-----------------------------------------------------------------------------
void GraphicsSystem::AddObject(const std::shared_ptr<IGraphicsObject>& object)
{
	m_objects.push_back(object->GetName(), object, 1);
}
//-----------------------------------------------------------------------------