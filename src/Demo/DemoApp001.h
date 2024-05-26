#pragma once

#include "DA001_HalfCornellBox.h"
#include "DA001_GBufferPass.h"

class DemoApp001 final : public EngineApp
{
public:
	void Run();

private:
	glm::mat4 m_perspective;
	RenderCamera m_camera;
};