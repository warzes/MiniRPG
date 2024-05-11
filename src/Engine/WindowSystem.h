#pragma once

#include "Systems.h"

class WindowSystemEvents
{
public:
	~WindowSystemEvents() = default;

	virtual void Resize(uint32_t /*width*/, uint32_t /*height*/) {}
};

struct WindowSystemCreateInfo final
{
	WindowSystemEvents* windowEventsHandle = nullptr;
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

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

private:
	Systems& m_systems;

	WindowSystemEvents* m_eventsHandle = nullptr;

	HINSTANCE m_hInstance = nullptr;
	HWND m_hwnd = nullptr;
	MSG m_msg{};
	HGLRC m_glContext = nullptr;
	HDC m_deviceContext = nullptr;

	uint32_t m_frameWidth = 0;
	uint32_t m_frameHeight = 0;
	bool m_shouldQuit = false;
};