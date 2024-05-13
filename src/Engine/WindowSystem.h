#pragma once

#include "Systems.h"
#include "PlatformEvent.h"
#include "EngineString.h"

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
	String title = L"Game";
	bool fullscreen = false;
	bool resize = true;
};

struct WindowImpl;

class WindowSystem final
{
	friend struct WindowImpl;
public:
	WindowSystem(Systems& systems);
	~WindowSystem();
	bool Create(const WindowSystemCreateInfo& createInfo);
	void Destroy();

	bool PeekMessages();
	[[nodiscard]] Event PollEvent();
	void SwapBuffers();
	bool ShouldQuit() const;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	HWND GetNativeHandle() const;

	glm::ivec2 GetPosition() const;
	void SetPosition(const glm::ivec2& position);

	glm::uvec2 GetSize() const;
	void SetSize(const glm::uvec2& size);

	void SetTitle(const String& title);

	void SetIcon(const glm::uvec2& size, const std::uint8_t* pixels);

	void SetVisible(bool visible);

	void SetMouseCursorVisible(bool visible);
	void SetMouseCursorGrabbed(bool grabbed);
	void SetKeyRepeatEnabled(bool enabled);

	bool HasFocus() const;

private:
	bool loadGLContext();
	Event popEvent(bool block);
	void processEvents();
	void filterEvent(const Event& event);
	LRESULT processEvent(UINT message, WPARAM wParam, LPARAM lParam);
	void pushEvent(const Event& event);
	void grabCursor(bool grabbed);
	glm::ivec2 contentSizeToWindowSize(const glm::uvec2& size);
	static Keyboard::Key virtualKeyCodeToEngine(WPARAM key, LPARAM flags);
	static Keyboard::Scancode toScancode(WPARAM wParam, LPARAM lParam);
	void setTracking(bool track);

	Systems& m_systems;
	WindowImpl* m_impl = nullptr;

	WindowSystemEvents* m_eventsHandle = nullptr;

	HINSTANCE m_hInstance = nullptr;
	HWND m_handle = nullptr;
	MSG m_message{};
	HGLRC m_glContext = nullptr;
	HDC m_deviceContext = nullptr;

	uint32_t m_frameWidth = 0;
	uint32_t m_frameHeight = 0;
	bool m_shouldQuit = false;

	std::queue<Event> m_events; // Queue of available events

	bool m_cursorVisible = true; // Is the cursor visible or hidden?
	HCURSOR  m_lastCursor{ LoadCursor(nullptr, IDC_ARROW) }; // Last cursor used -- this data is not owned by the window and is required to be always valid
	HICON m_icon{}; // Custom icon assigned to the window
	bool m_keyRepeatEnabled = true; // Automatic key-repeat state for keydown events
	glm::uvec2 m_lastSize; // The last handled size of the window
	bool m_resizing{}; // Is the window being resized?
	std::uint16_t m_surrogate{}; // First half of the surrogate pair, in case we're receiving a Unicode character in two events
	bool m_mouseInside{}; // Mouse is inside the window?
	bool m_fullscreen{}; // Is the window fullscreen?
	bool m_cursorGrabbed = false; // Is the mouse cursor trapped?
};