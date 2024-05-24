#pragma once

class DemoApp001 final : public EngineApp
{
public:
	void Run();

private:
	glm::mat4 m_perspective;
	RenderCamera m_camera;
};