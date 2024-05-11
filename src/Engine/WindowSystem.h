#pragma once

#include "Systems.h"

struct WindowSystemCreateInfo final
{
	int width = 1024;
	int height = 768;
	const wchar_t* title = L"Game";
	bool fullscreen = false;
	bool resize = true;
};

class WindowSystem final
{
public:
	WindowSystem(Systems& systems);
	~WindowSystem();
	bool Create(const WindowSystemCreateInfo& createInfo);
	void Destroy();

	bool PeekMessages();
	void SwapBuffers();
	bool ShouldQuit() const;

private:
	Systems& m_systems;

	HINSTANCE m_hInstance = nullptr;
	HWND m_hwnd = nullptr;
	MSG m_msg{};
	HGLRC m_glContext = nullptr;
	HDC m_deviceContext = nullptr;

	uint32_t m_frameWidth = 0;
	uint32_t m_frameHeight = 0;
	bool m_shouldQuit = false;
};