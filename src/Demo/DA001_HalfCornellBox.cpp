#include "stdafx.h"
#include "DA001_HalfCornellBox.h"
#include "GraphicsSystem.h"

DA001_HalfCornellBox::DA001_HalfCornellBox(Systems& systems, const std::string& gameObjectName, int executionOrder)
	: IGraphicsObject(systems, gameObjectName, executionOrder)
{
}

void DA001_HalfCornellBox::Init()
{
	SetModel(m_systems.graphics->GetModel("Data/models/sponza/sponza.obj"));
}