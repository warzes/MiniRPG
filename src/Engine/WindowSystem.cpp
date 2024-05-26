#include "stdafx.h"
#include "Core.h"
#include "WindowSystem.h"
#include "LogSystem.h"
#include "EngineTime.h"
#include "Sleep.h"
#include "TemporaryWindow.h"
#include "MainGUISystem.h"
//-----------------------------------------------------------------------------
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "winmm.lib")
//-----------------------------------------------------------------------------
constexpr const wchar_t* WndClassName = L"MainWindowMiniEngine";
//-----------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//-----------------------------------------------------------------------------
struct WindowImpl final
{
	WindowImpl(WindowSystem* wnd) : window(wnd) {}

	LRESULT processEvent(UINT message, WPARAM wParam, LPARAM lParam)
	{
		return window->processEvent(message, wParam, lParam);
	}

	WindowSystem* window = nullptr;
};
//-----------------------------------------------------------------------------
void setProcessDpiAware()
{
	// Try SetProcessDpiAwareness first
	HINSTANCE shCoreDll = LoadLibrary(L"Shcore.dll");

	if (shCoreDll)
	{
		enum ProcessDpiAwareness
		{
			ProcessDpiUnaware = 0,
			ProcessSystemDpiAware = 1,
			ProcessPerMonitorDpiAware = 2
		};

		using SetProcessDpiAwarenessFuncType = HRESULT(WINAPI*)(ProcessDpiAwareness);
		auto setProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(reinterpret_cast<void*>(GetProcAddress(shCoreDll, "SetProcessDpiAwareness")));

		if (setProcessDpiAwarenessFunc)
		{
			// We only check for E_INVALIDARG because we would get
			// E_ACCESSDENIED if the DPI was already set previously
			// and S_OK means the call was successful.
			// We intentionally don't use Per Monitor V2 which can be
			// enabled with SetProcessDpiAwarenessContext, because that
			// would scale the title bar and thus change window size
			// by default when moving the window between monitors.
			if (setProcessDpiAwarenessFunc(ProcessPerMonitorDpiAware) == E_INVALIDARG)
			{
				LogError("Failed to set process DPI awareness");
			}
			else
			{
				FreeLibrary(shCoreDll);
				return;
			}
		}

		FreeLibrary(shCoreDll);
	}

	// Fall back to SetProcessDPIAware if SetProcessDpiAwareness
	// is not available on this system
	HINSTANCE user32Dll = LoadLibrary(L"user32.dll");

	if (user32Dll)
	{
		using SetProcessDPIAwareFuncType = BOOL(WINAPI*)();
		auto setProcessDPIAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(
			reinterpret_cast<void*>(GetProcAddress(user32Dll, "SetProcessDPIAware")));

		if (setProcessDPIAwareFunc)
		{
			if (!setProcessDPIAwareFunc())
				LogError("Failed to set process DPI awareness");
		}

		FreeLibrary(user32Dll);
	}
}
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	WindowImpl* windowImpl = hWnd ? reinterpret_cast<WindowImpl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)) : nullptr;
	if (windowImpl)
	{
		return windowImpl->processEvent(message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//-----------------------------------------------------------------------------
WindowSystem::WindowSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.window = this;
	m_impl = new WindowImpl(this);
}
//-----------------------------------------------------------------------------
WindowSystem::~WindowSystem()
{
	Destroy();
	delete m_impl;
}
//-----------------------------------------------------------------------------
bool WindowSystem::Create(const WindowSystemCreateInfo& createInfo)
{
	assert(m_systems.log);
	assert(!m_hInstance);

	m_hInstance = GetModuleHandle(nullptr);

	m_fullscreen = createInfo.fullscreen;

	setProcessDpiAware();

	{
		TemporaryWindow tempWindow(*m_systems.log, m_hInstance);
		if (tempWindow.success == false)
		{
			m_systems.log->Fatal("WindowSystem: Failed to create temporary window");
			return false;
		}
	}

	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = m_hInstance;
	wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wndClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wndClass.lpszClassName = WndClassName;
	if (RegisterClassEx(&wndClass) == 0)
	{
		m_systems.log->Fatal("Windows failed to register class");
		return false;
	}

	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.top = 0L;
	windowRect.right = (long)createInfo.width;
	windowRect.bottom = (long)createInfo.height;
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	m_handle = CreateWindowEx(0, WndClassName, createInfo.title.ToWideString().c_str(), 
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, m_hInstance, nullptr);
	if (!m_handle)
	{
		m_systems.log->Fatal("Windows failed to create window");
		return false;
	}

	// Center on screen
	{		
		uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
		uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
		SetWindowPos(m_handle, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	GetClientRect(m_handle, &windowRect);
	m_frameWidth = windowRect.right - windowRect.left;
	m_frameHeight = windowRect.bottom - windowRect.top;
	m_lastSize = { m_frameWidth, m_frameHeight };

	if (!loadGLContext())
		return false;

	SetWindowLongPtrW(m_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(m_impl));

	SetVisible(true);
	SetMouseCursorVisible(true);
	SetKeyRepeatEnabled(true);

	m_shouldQuit = false;

	return true;
}
//-----------------------------------------------------------------------------
void WindowSystem::Destroy()
{
	SetMouseCursorVisible(true);
	setTracking(false);
	ReleaseCapture();
	if (m_glContext && m_deviceContext) wglMakeCurrent(nullptr, nullptr);
	if (m_glContext) wglDeleteContext(m_glContext);
	if (m_deviceContext) ReleaseDC(m_handle, m_deviceContext);
	if (m_handle) DestroyWindow(m_handle);
	if (m_hInstance) UnregisterClass(WndClassName, m_hInstance);

	m_glContext = nullptr;
	m_deviceContext = nullptr;
	m_handle = nullptr;
	m_hInstance = nullptr;
	m_systems.window = nullptr;
}
//-----------------------------------------------------------------------------
bool WindowSystem::PeekMessages()
{
	if (PeekMessage(&m_message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_message);
		DispatchMessage(&m_message);
		if (m_message.message == WM_QUIT)
			m_shouldQuit = true;

		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
Event WindowSystem::PollEvent()
{
	Event event;
	if (!m_shouldQuit && (event = popEvent(false)))
		filterEvent(event);
	return event;
}
//-----------------------------------------------------------------------------
void WindowSystem::SwapBuffers()
{
	::SwapBuffers(m_deviceContext);
}
//-----------------------------------------------------------------------------
bool WindowSystem::ShouldQuit() const
{
	return m_shouldQuit;
}
//-----------------------------------------------------------------------------
uint32_t WindowSystem::GetWidth() const
{
	return m_frameWidth;
}
//-----------------------------------------------------------------------------
uint32_t WindowSystem::GetHeight() const
{
	return m_frameHeight;
}
//-----------------------------------------------------------------------------
HWND WindowSystem::GetNativeHandle() const
{
	return m_handle;
}
//-----------------------------------------------------------------------------
glm::ivec2 WindowSystem::GetPosition() const
{
	RECT rect;
	GetWindowRect(m_handle, &rect);
	return { rect.left, rect.top };
}
//-----------------------------------------------------------------------------
void WindowSystem::SetPosition(const glm::ivec2& position)
{
	SetWindowPos(m_handle, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	if (m_cursorGrabbed)
		grabCursor(true);
}
//-----------------------------------------------------------------------------
glm::uvec2 WindowSystem::GetSize() const
{
	RECT rect;
	GetClientRect(m_handle, &rect);
	return glm::uvec2(rect.right - rect.left, rect.bottom - rect.top);
}
//-----------------------------------------------------------------------------
void WindowSystem::SetSize(const glm::uvec2& size)
{
	const auto [width, height] = contentSizeToWindowSize(size);
	SetWindowPos(m_handle, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}
//-----------------------------------------------------------------------------
void WindowSystem::SetTitle(const String& title)
{
	SetWindowTextW(m_handle, title.ToWideString().c_str());
}
//-----------------------------------------------------------------------------
void WindowSystem::SetIcon(const glm::uvec2& size, const std::uint8_t* pixels)
{
	// First destroy the previous one
	if (m_icon)
		DestroyIcon(m_icon);

	// Windows wants BGRA pixels: swap red and blue channels
	std::vector<std::uint8_t> iconPixels(size.x * size.y * 4);
	for (std::size_t i = 0; i < iconPixels.size() / 4; ++i)
	{
		iconPixels[i * 4 + 0] = pixels[i * 4 + 2];
		iconPixels[i * 4 + 1] = pixels[i * 4 + 1];
		iconPixels[i * 4 + 2] = pixels[i * 4 + 0];
		iconPixels[i * 4 + 3] = pixels[i * 4 + 3];
	}

	// Create the icon from the pixel array
	m_icon = CreateIcon(GetModuleHandleW(nullptr),
		static_cast<int>(size.x),
		static_cast<int>(size.y),
		1,
		32,
		nullptr,
		iconPixels.data());

	// Set it as both big and small icon of the window
	if (m_icon)
	{
		SendMessageW(m_handle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(m_icon));
		SendMessageW(m_handle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(m_icon));
	}
	else
	{
		LogError("Failed to set the window's icon");
	}
}
//-----------------------------------------------------------------------------
void WindowSystem::SetVisible(bool visible)
{
	ShowWindow(m_handle, visible ? SW_SHOW : SW_HIDE);
}
//-----------------------------------------------------------------------------
void WindowSystem::SetMouseCursorVisible(bool visible)
{
	m_cursorVisible = visible;
	SetCursor(m_cursorVisible ? m_lastCursor : nullptr);
}
//-----------------------------------------------------------------------------
void WindowSystem::SetMouseCursorGrabbed(bool grabbed)
{
	m_cursorGrabbed = grabbed;
	grabCursor(m_cursorGrabbed);
}
//-----------------------------------------------------------------------------
void WindowSystem::SetKeyRepeatEnabled(bool enabled)
{
	m_keyRepeatEnabled = enabled;
}
//-----------------------------------------------------------------------------
bool WindowSystem::HasFocus() const
{
	return m_handle == GetForegroundWindow();
}
//-----------------------------------------------------------------------------
bool WindowSystem::loadGLContext()
{
	m_deviceContext = GetDC(m_handle);
	if (m_deviceContext == nullptr)
	{
		m_systems.log->Fatal("GetDC() failed!");
		return false;
	}

	int pixel_format = 0;
	unsigned int num_pixel_format = 0;
	const int pixel_attribs[] =
	{
		WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,
		WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
		WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,		32,
		WGL_DEPTH_BITS_ARB,		24,
		WGL_STENCIL_BITS_ARB,	8,
		WGL_SAMPLE_BUFFERS_ARB,	GL_TRUE,
		WGL_SAMPLES_ARB,		4,
		0
	};
	if (!wglChoosePixelFormatARB(m_deviceContext, pixel_attribs, nullptr, 1, &pixel_format, &num_pixel_format))
	{
		m_systems.log->Fatal("Failed to choose pixel format!");
		return false;
	}

	PIXELFORMATDESCRIPTOR pfd2;
	if (DescribePixelFormat(m_deviceContext, pixel_format, sizeof(pfd2), &pfd2) == 0)
	{
		m_systems.log->Fatal("Failed to describe pixel format!");
		return false;
	}

	if (!SetPixelFormat(m_deviceContext, pixel_format, &pfd2))
	{
		m_systems.log->Fatal("Unable to set pixel format!");
		return false;
	}

	// 4.6 forward compatible core profile
	int contextFlags = WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
#ifdef _DEBUG
	contextFlags |= WGL_CONTEXT_DEBUG_BIT_ARB;
#else
	if (GLAD_WGL_ARB_create_context_no_error)
		contextFlags |= WGL_CONTEXT_OPENGL_NO_ERROR_ARB;
#endif

	const int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_LAYER_PLANE_ARB, 0, // main plane
		WGL_CONTEXT_FLAGS_ARB, contextFlags,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	m_glContext = wglCreateContextAttribsARB(m_deviceContext, nullptr, attribs);
	if (m_glContext == nullptr)
	{
		m_systems.log->Fatal("Could not create wgl context!");
		return false;
	}

	wglMakeCurrent(m_deviceContext, m_glContext);

	int gladSuccess = gladLoaderLoadGL();
	gladLoaderUnloadGL();
	if (!gladSuccess)
	{
		m_systems.log->Fatal("glad: Failed load gl Func");
		return false;
	}

	return true;
}
//-----------------------------------------------------------------------------
Event WindowSystem::popEvent(bool block)
{
	// If the event queue is empty, let's first check if new events are available from the OS
	if (m_events.empty())
	{
		// Get events from the system
		processEvents();

		// In blocking mode, we must process events until one is triggered
		if (block)
		{
			// Here we use a manual wait loop instead of the optimized wait-event provided by the OS, so that we don't skip joystick events (which require polling)
			while (m_events.empty())
			{
				Sleep(Milliseconds(10));
				processEvents();
			}
		}
	}

	Event event;
	// Pop the first event of the queue, if it is not empty
	if (!m_events.empty())
	{
		event = m_events.front();
		m_events.pop();
	}

	return event;
}
//-----------------------------------------------------------------------------
void WindowSystem::processEvents()
{
	while (PeekMessage(&m_message, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_message);
		DispatchMessage(&m_message);
		if (m_message.message == WM_QUIT)
			m_shouldQuit = true;
	}
}
//-----------------------------------------------------------------------------
void WindowSystem::filterEvent(const Event& event)
{
	// Notify resize events to the derived class
	if (const auto* resized = event.GetIf<Event::Resized>())
	{
		// Cache the new size
		m_frameWidth = resized->size.x;
		m_frameHeight = resized->size.y;

		// Notify the derived class
		if (m_eventsHandle) m_eventsHandle->Resize(m_frameWidth, m_frameHeight);
	}
}
//-----------------------------------------------------------------------------
LRESULT WindowSystem::processEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU))
		return 0;

	if (m_systems.mainGUI && m_systems.mainGUI->IsEnable())
	{
		if (ImGui_ImplWin32_WndProcHandler(m_handle, message, wParam, lParam))
			return 0;
	}

	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_CLOSE:
		pushEvent(Event::Closed{});
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SETCURSOR:
	{
		// The mouse has moved, if the cursor is in our window we must refresh the cursor
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(m_cursorVisible ? m_lastCursor : nullptr);
		}

		break;
	}

	// Resize event
	case WM_SIZE:
	{
		// Consider only events triggered by a maximize or a un-maximize
		if (wParam != SIZE_MINIMIZED && !m_resizing && m_lastSize != GetSize())
		{
			// Update the last handled size
			m_lastSize = GetSize();

			// Push a resize event
			pushEvent(Event::Resized{ m_lastSize });

			// Restore/update cursor grabbing
			grabCursor(m_cursorGrabbed);
		}
		break;
	}

	// Start resizing
	case WM_ENTERSIZEMOVE:
	{
		m_resizing = true;
		grabCursor(false);
		break;
	}

	// Stop resizing
	case WM_EXITSIZEMOVE:
	{
		m_resizing = false;

		// Ignore cases where the window has only been moved
		if (m_lastSize != GetSize())
		{
			// Update the last handled size
			m_lastSize = GetSize();

			// Push a resize event
			pushEvent(Event::Resized{ m_lastSize });
		}

		// Restore/update cursor grabbing
		grabCursor(m_cursorGrabbed);
		break;
	}

	// Fix violations of minimum or maximum size
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO minMaxInfo = reinterpret_cast<MINMAXINFO*>(lParam);
		minMaxInfo->ptMinTrackSize.x = 320;
		minMaxInfo->ptMinTrackSize.y = 320;
		break;
	}

	// Gain focus event
	case WM_SETFOCUS:
	{
		// Restore cursor grabbing
		grabCursor(m_cursorGrabbed);

		pushEvent(Event::FocusGained{});
		break;
	}

	// Lost focus event
	case WM_KILLFOCUS:
	{
		// Ungrab the cursor
		grabCursor(false);

		pushEvent(Event::FocusLost{});
		break;
	}

	// Text event
	case WM_CHAR:
	{
		if (m_keyRepeatEnabled || ((lParam & (1 << 30)) == 0))
		{
			// Get the code of the typed character
			auto character = static_cast<std::uint32_t>(wParam);

			// Check if it is the first part of a surrogate pair, or a regular character
			if ((character >= 0xD800) && (character <= 0xDBFF))
			{
				// First part of a surrogate pair: store it and wait for the second one
				m_surrogate = static_cast<std::uint16_t>(character);
			}
			else
			{
				// Check if it is the second part of a surrogate pair, or a regular character
				if ((character >= 0xDC00) && (character <= 0xDFFF))
				{
					// Convert the UTF-16 surrogate pair to a single UTF-32 value
					std::uint16_t utf16[] = { m_surrogate, static_cast<std::uint16_t>(character) };
					Utf16::toUtf32(utf16, utf16 + 2, &character);
					m_surrogate = 0;
				}

				// Send a TextEntered event
				pushEvent(Event::TextEntered{ character });
			}
		}
		break;
	}

	// Keydown event
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	{
		if (m_keyRepeatEnabled || ((HIWORD(lParam) & KF_REPEAT) == 0))
		{
			Event::KeyPressed event;
			event.alt = HIWORD(GetKeyState(VK_MENU)) != 0;
			event.control = HIWORD(GetKeyState(VK_CONTROL)) != 0;
			event.shift = HIWORD(GetKeyState(VK_SHIFT)) != 0;
			event.system = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
			event.code = virtualKeyCodeToEngine(wParam, lParam);
			event.scancode = toScancode(wParam, lParam);
			pushEvent(event);
		}
		break;
	}

	// Keyup event
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
		Event::KeyReleased event;
		event.alt = HIWORD(GetKeyState(VK_MENU)) != 0;
		event.control = HIWORD(GetKeyState(VK_CONTROL)) != 0;
		event.shift = HIWORD(GetKeyState(VK_SHIFT)) != 0;
		event.system = HIWORD(GetKeyState(VK_LWIN)) || HIWORD(GetKeyState(VK_RWIN));
		event.code = virtualKeyCodeToEngine(wParam, lParam);
		event.scancode = toScancode(wParam, lParam);
		pushEvent(event);
		break;
	}

	// Vertical mouse wheel event
	case WM_MOUSEWHEEL:
	{
		// Mouse position is in screen coordinates, convert it to window coordinates
		POINT position;
		position.x = static_cast<std::int16_t>(LOWORD(lParam));
		position.y = static_cast<std::int16_t>(HIWORD(lParam));
		ScreenToClient(m_handle, &position);

		auto delta = static_cast<std::int16_t>(HIWORD(wParam));

		Event::MouseWheelScrolled event;
		event.wheel = Mouse::Wheel::Vertical;
		event.delta = static_cast<float>(delta) / 120.f;
		event.position = { position.x, position.y };
		pushEvent(event);
		break;
	}

	// Horizontal mouse wheel event
	case WM_MOUSEHWHEEL:
	{
		// Mouse position is in screen coordinates, convert it to window coordinates
		POINT position;
		position.x = static_cast<std::int16_t>(LOWORD(lParam));
		position.y = static_cast<std::int16_t>(HIWORD(lParam));
		ScreenToClient(m_handle, &position);

		auto delta = static_cast<std::int16_t>(HIWORD(wParam));

		Event::MouseWheelScrolled event;
		event.wheel = Mouse::Wheel::Horizontal;
		event.delta = -static_cast<float>(delta) / 120.f;
		event.position = { position.x, position.y };
		pushEvent(event);
		break;
	}

	// Mouse left button down event
	case WM_LBUTTONDOWN:
	{
		Event::MouseButtonPressed event;
		event.button = Mouse::Button::Left;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse left button up event
	case WM_LBUTTONUP:
	{
		Event::MouseButtonReleased event;
		event.button = Mouse::Button::Left;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse right button down event
	case WM_RBUTTONDOWN:
	{
		Event::MouseButtonPressed event;
		event.button = Mouse::Button::Right;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse right button up event
	case WM_RBUTTONUP:
	{
		Event::MouseButtonReleased event;
		event.button = Mouse::Button::Right;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse wheel button down event
	case WM_MBUTTONDOWN:
	{
		Event::MouseButtonPressed event;
		event.button = Mouse::Button::Middle;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse wheel button up event
	case WM_MBUTTONUP:
	{
		Event::MouseButtonReleased event;
		event.button = Mouse::Button::Middle;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse X button down event
	case WM_XBUTTONDOWN:
	{
		Event::MouseButtonPressed event;
		event.button = HIWORD(wParam) == XBUTTON1 ? Mouse::Button::Extra1 : Mouse::Button::Extra2;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse X button up event
	case WM_XBUTTONUP:
	{
		Event::MouseButtonReleased event;
		event.button = HIWORD(wParam) == XBUTTON1 ? Mouse::Button::Extra1 : Mouse::Button::Extra2;
		event.position = { static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)) };
		pushEvent(event);
		break;
	}

	// Mouse leave event
	case WM_MOUSELEAVE:
	{
		// Avoid this firing a second time in case the cursor is dragged outside
		if (m_mouseInside)
		{
			m_mouseInside = false;

			// Generate a MouseLeft event
			pushEvent(Event::MouseLeft{});
		}
		break;
	}

	// Mouse move event
	case WM_MOUSEMOVE:
	{
		// Extract the mouse local coordinates
		const int x = static_cast<std::int16_t>(LOWORD(lParam));
		const int y = static_cast<std::int16_t>(HIWORD(lParam));

		// Get the client area of the window
		RECT area;
		GetClientRect(m_handle, &area);

		// Capture the mouse in case the user wants to drag it outside
		if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
		{
			// Only release the capture if we really have it
			if (GetCapture() == m_handle)
				ReleaseCapture();
		}
		else if (GetCapture() != m_handle)
		{
			// Set the capture to continue receiving mouse events
			SetCapture(m_handle);
		}

		// If the cursor is outside the client area...
		if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom))
		{
			// and it used to be inside, the mouse left it.
			if (m_mouseInside)
			{
				m_mouseInside = false;

				// No longer care for the mouse leaving the window
				setTracking(false);

				// Generate a MouseLeft event
				pushEvent(Event::MouseLeft{});
			}
		}
		else
		{
			// and vice-versa
			if (!m_mouseInside)
			{
				m_mouseInside = true;

				// Look for the mouse leaving the window
				setTracking(true);

				// Generate a MouseEntered event
				pushEvent(Event::MouseEntered{});
			}
		}

		// Generate a MouseMove event
		pushEvent(Event::MouseMoved{ {x, y} });
		break;
	}

	}
	return DefWindowProc(m_handle, message, wParam, lParam);
}
//-----------------------------------------------------------------------------
void WindowSystem::pushEvent(const Event& event)
{
	m_events.push(event);
}
//-----------------------------------------------------------------------------
void WindowSystem::grabCursor(bool grabbed)
{
	if (grabbed)
	{
		RECT rect;
		GetClientRect(m_handle, &rect);
		MapWindowPoints(m_handle, nullptr, reinterpret_cast<LPPOINT>(&rect), 2);
		ClipCursor(&rect);
	}
	else
	{
		ClipCursor(nullptr);
	}
}
//-----------------------------------------------------------------------------
glm::ivec2 WindowSystem::contentSizeToWindowSize(const glm::uvec2& size)
{
	// SetWindowPos wants the total size of the window (including title bar and borders) so we have to compute it
	RECT rectangle = { 0, 0, static_cast<long>(size.x), static_cast<long>(size.y) };
	AdjustWindowRect(&rectangle, static_cast<DWORD>(GetWindowLongPtr(m_handle, GWL_STYLE)), false);
	const auto width = rectangle.right - rectangle.left;
	const auto height = rectangle.bottom - rectangle.top;
	return { width, height };
}
//-----------------------------------------------------------------------------
Keyboard::Key WindowSystem::virtualKeyCodeToEngine(WPARAM key, LPARAM flags)
{
	switch (key)
	{
	// Check the scancode to distinguish between left and right shift
	case VK_SHIFT:
	{
		static const UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
		const UINT scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
		return scancode == lShift ? Keyboard::Key::LShift : Keyboard::Key::RShift;
	}

	// Check the "extended" flag to distinguish between left and right alt
	case VK_MENU: return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::Key::RAlt : Keyboard::Key::LAlt;

	// Check the "extended" flag to distinguish between left and right control
	case VK_CONTROL: return (HIWORD(flags) & KF_EXTENDED) ? Keyboard::Key::RControl : Keyboard::Key::LControl;

	// Other keys are reported properly
	case VK_LWIN:       return Keyboard::Key::LSystem;
	case VK_RWIN:       return Keyboard::Key::RSystem;
	case VK_APPS:       return Keyboard::Key::Menu;
	case VK_OEM_1:      return Keyboard::Key::Semicolon;
	case VK_OEM_2:      return Keyboard::Key::Slash;
	case VK_OEM_PLUS:   return Keyboard::Key::Equal;
	case VK_OEM_MINUS:  return Keyboard::Key::Hyphen;
	case VK_OEM_4:      return Keyboard::Key::LBracket;
	case VK_OEM_6:      return Keyboard::Key::RBracket;
	case VK_OEM_COMMA:  return Keyboard::Key::Comma;
	case VK_OEM_PERIOD: return Keyboard::Key::Period;
	case VK_OEM_7:      return Keyboard::Key::Apostrophe;
	case VK_OEM_5:      return Keyboard::Key::Backslash;
	case VK_OEM_3:      return Keyboard::Key::Grave;
	case VK_ESCAPE:     return Keyboard::Key::Escape;
	case VK_SPACE:      return Keyboard::Key::Space;
	case VK_RETURN:     return Keyboard::Key::Enter;
	case VK_BACK:       return Keyboard::Key::Backspace;
	case VK_TAB:        return Keyboard::Key::Tab;
	case VK_PRIOR:      return Keyboard::Key::PageUp;
	case VK_NEXT:       return Keyboard::Key::PageDown;
	case VK_END:        return Keyboard::Key::End;
	case VK_HOME:       return Keyboard::Key::Home;
	case VK_INSERT:     return Keyboard::Key::Insert;
	case VK_DELETE:     return Keyboard::Key::Delete;
	case VK_ADD:        return Keyboard::Key::Add;
	case VK_SUBTRACT:   return Keyboard::Key::Subtract;
	case VK_MULTIPLY:   return Keyboard::Key::Multiply;
	case VK_DIVIDE:     return Keyboard::Key::Divide;
	case VK_PAUSE:      return Keyboard::Key::Pause;
	case VK_F1:         return Keyboard::Key::F1;
	case VK_F2:         return Keyboard::Key::F2;
	case VK_F3:         return Keyboard::Key::F3;
	case VK_F4:         return Keyboard::Key::F4;
	case VK_F5:         return Keyboard::Key::F5;
	case VK_F6:         return Keyboard::Key::F6;
	case VK_F7:         return Keyboard::Key::F7;
	case VK_F8:         return Keyboard::Key::F8;
	case VK_F9:         return Keyboard::Key::F9;
	case VK_F10:        return Keyboard::Key::F10;
	case VK_F11:        return Keyboard::Key::F11;
	case VK_F12:        return Keyboard::Key::F12;
	case VK_F13:        return Keyboard::Key::F13;
	case VK_F14:        return Keyboard::Key::F14;
	case VK_F15:        return Keyboard::Key::F15;
	case VK_LEFT:       return Keyboard::Key::Left;
	case VK_RIGHT:      return Keyboard::Key::Right;
	case VK_UP:         return Keyboard::Key::Up;
	case VK_DOWN:       return Keyboard::Key::Down;
	case VK_NUMPAD0:    return Keyboard::Key::Numpad0;
	case VK_NUMPAD1:    return Keyboard::Key::Numpad1;
	case VK_NUMPAD2:    return Keyboard::Key::Numpad2;
	case VK_NUMPAD3:    return Keyboard::Key::Numpad3;
	case VK_NUMPAD4:    return Keyboard::Key::Numpad4;
	case VK_NUMPAD5:    return Keyboard::Key::Numpad5;
	case VK_NUMPAD6:    return Keyboard::Key::Numpad6;
	case VK_NUMPAD7:    return Keyboard::Key::Numpad7;
	case VK_NUMPAD8:    return Keyboard::Key::Numpad8;
	case VK_NUMPAD9:    return Keyboard::Key::Numpad9;
	case 'A':           return Keyboard::Key::A;
	case 'Z':           return Keyboard::Key::Z;
	case 'E':           return Keyboard::Key::E;
	case 'R':           return Keyboard::Key::R;
	case 'T':           return Keyboard::Key::T;
	case 'Y':           return Keyboard::Key::Y;
	case 'U':           return Keyboard::Key::U;
	case 'I':           return Keyboard::Key::I;
	case 'O':           return Keyboard::Key::O;
	case 'P':           return Keyboard::Key::P;
	case 'Q':           return Keyboard::Key::Q;
	case 'S':           return Keyboard::Key::S;
	case 'D':           return Keyboard::Key::D;
	case 'F':           return Keyboard::Key::F;
	case 'G':           return Keyboard::Key::G;
	case 'H':           return Keyboard::Key::H;
	case 'J':           return Keyboard::Key::J;
	case 'K':           return Keyboard::Key::K;
	case 'L':           return Keyboard::Key::L;
	case 'M':           return Keyboard::Key::M;
	case 'W':           return Keyboard::Key::W;
	case 'X':           return Keyboard::Key::X;
	case 'C':           return Keyboard::Key::C;
	case 'V':           return Keyboard::Key::V;
	case 'B':           return Keyboard::Key::B;
	case 'N':           return Keyboard::Key::N;
	case '0':           return Keyboard::Key::Num0;
	case '1':           return Keyboard::Key::Num1;
	case '2':           return Keyboard::Key::Num2;
	case '3':           return Keyboard::Key::Num3;
	case '4':           return Keyboard::Key::Num4;
	case '5':           return Keyboard::Key::Num5;
	case '6':           return Keyboard::Key::Num6;
	case '7':           return Keyboard::Key::Num7;
	case '8':           return Keyboard::Key::Num8;
	case '9':           return Keyboard::Key::Num9;
	}

	return Keyboard::Key::Unknown;
}
//-----------------------------------------------------------------------------
Keyboard::Scancode WindowSystem::toScancode(WPARAM wParam, LPARAM lParam)
{
	int code = (lParam & (0xFF << 16)) >> 16;

	// Retrieve the scancode from the VirtualKey for synthetic key messages
	if (code == 0)
	{
		code = static_cast<int>(MapVirtualKey(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC));
	}

	// Windows scancodes
	// Reference: https://msdn.microsoft.com/en-us/library/aa299374(v=vs.60).aspx
	switch (code)
	{
	case 1: return Keyboard::Scancode::Escape;
	case 2: return Keyboard::Scancode::Num1;
	case 3: return Keyboard::Scancode::Num2;
	case 4: return Keyboard::Scancode::Num3;
	case 5: return Keyboard::Scancode::Num4;
	case 6: return Keyboard::Scancode::Num5;
	case 7: return Keyboard::Scancode::Num6;
	case 8: return Keyboard::Scancode::Num7;
	case 9: return Keyboard::Scancode::Num8;
	case 10: return Keyboard::Scancode::Num9;
	case 11: return Keyboard::Scancode::Num0;
	case 12: return Keyboard::Scancode::Hyphen;
	case 13: return Keyboard::Scancode::Equal;
	case 14: return Keyboard::Scancode::Backspace;
	case 15: return Keyboard::Scancode::Tab;
	case 16: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::MediaPreviousTrack : Keyboard::Scancode::Q;
	case 17: return Keyboard::Scancode::W;
	case 18: return Keyboard::Scancode::E;
	case 19: return Keyboard::Scancode::R;
	case 20: return Keyboard::Scancode::T;
	case 21: return Keyboard::Scancode::Y;
	case 22: return Keyboard::Scancode::U;
	case 23: return Keyboard::Scancode::I;
	case 24: return Keyboard::Scancode::O;
	case 25: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::MediaNextTrack : Keyboard::Scancode::P;
	case 26: return Keyboard::Scancode::LBracket;
	case 27: return Keyboard::Scancode::RBracket;
	case 28: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::NumpadEnter : Keyboard::Scancode::Enter;
	case 29: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::RControl : Keyboard::Scancode::LControl;
	case 30: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Select : Keyboard::Scancode::A;
	case 31: return Keyboard::Scancode::S;
	case 32: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::VolumeMute : Keyboard::Scancode::D;
	case 33: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::LaunchApplication1 : Keyboard::Scancode::F;
	case 34: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::MediaPlayPause : Keyboard::Scancode::G;
	case 35: return Keyboard::Scancode::H;
	case 36: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::MediaStop : Keyboard::Scancode::J;
	case 37: return Keyboard::Scancode::K;
	case 38: return Keyboard::Scancode::L;
	case 39: return Keyboard::Scancode::Semicolon;
	case 40: return Keyboard::Scancode::Apostrophe;
	case 41: return Keyboard::Scancode::Grave;
	case 42: return Keyboard::Scancode::LShift;
	case 43: return Keyboard::Scancode::Backslash;
	case 44: return Keyboard::Scancode::Z;
	case 45: return Keyboard::Scancode::X;
	case 46: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::VolumeDown : Keyboard::Scancode::C;
	case 47: return Keyboard::Scancode::V;
	case 48: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::VolumeUp : Keyboard::Scancode::B;
	case 49: return Keyboard::Scancode::N;
	case 50: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::HomePage : Keyboard::Scancode::M;
	case 51: return Keyboard::Scancode::Comma;
	case 52: return Keyboard::Scancode::Period;
	case 53: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::NumpadDivide : Keyboard::Scancode::Slash;
	case 54: return Keyboard::Scancode::RShift;
	case 55: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::PrintScreen : Keyboard::Scancode::NumpadMultiply;
	case 56: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::RAlt : Keyboard::Scancode::LAlt;
	case 57: return Keyboard::Scancode::Space;
	case 58: return Keyboard::Scancode::CapsLock;
	case 59: return Keyboard::Scancode::F1;
	case 60: return Keyboard::Scancode::F2;
	case 61: return Keyboard::Scancode::F3;
	case 62: return Keyboard::Scancode::F4;
	case 63: return Keyboard::Scancode::F5;
	case 64: return Keyboard::Scancode::F6;
	case 65: return Keyboard::Scancode::F7;
	case 66: return Keyboard::Scancode::F8;
	case 67: return Keyboard::Scancode::F9;
	case 68: return Keyboard::Scancode::F10;
	case 69: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::NumLock : Keyboard::Scancode::Pause;
	case 70: return Keyboard::Scancode::ScrollLock;
	case 71: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Home : Keyboard::Scancode::Numpad7;
	case 72: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Up : Keyboard::Scancode::Numpad8;
	case 73: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::PageUp : Keyboard::Scancode::Numpad9;
	case 74: return Keyboard::Scancode::NumpadMinus;
	case 75: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Left : Keyboard::Scancode::Numpad4;
	case 76: return Keyboard::Scancode::Numpad5;
	case 77: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Right : Keyboard::Scancode::Numpad6;
	case 78: return Keyboard::Scancode::NumpadPlus;
	case 79: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::End : Keyboard::Scancode::Numpad1;
	case 80: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Down : Keyboard::Scancode::Numpad2;
	case 81: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::PageDown : Keyboard::Scancode::Numpad3;
	case 82: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Insert : Keyboard::Scancode::Numpad0;
	case 83: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Delete : Keyboard::Scancode::NumpadDecimal;

	case 86: return Keyboard::Scancode::NonUsBackslash;
	case 87: return Keyboard::Scancode::F11;
	case 88: return Keyboard::Scancode::F12;

	case 91: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::LSystem : Keyboard::Scancode::Unknown;
	case 92: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::RSystem : Keyboard::Scancode::Unknown;
	case 93: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Menu : Keyboard::Scancode::Unknown;

	case 99: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Help : Keyboard::Scancode::Unknown;
	case 100: return Keyboard::Scancode::F13;
	case 101: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Search : Keyboard::Scancode::F14;
	case 102: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Favorites : Keyboard::Scancode::F15;
	case 103: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Refresh : Keyboard::Scancode::F16;
	case 104: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Stop : Keyboard::Scancode::F17;
	case 105: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Forward : Keyboard::Scancode::F18;
	case 106: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::Back : Keyboard::Scancode::F19;
	case 107: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::LaunchApplication1 : Keyboard::Scancode::F20;
	case 108: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::LaunchMail : Keyboard::Scancode::F21;
	case 109: return (HIWORD(lParam) & KF_EXTENDED) ? Keyboard::Scancode::LaunchMediaSelect : Keyboard::Scancode::F22;
	case 110: return Keyboard::Scancode::F23;

	case 118: return Keyboard::Scancode::F24;

	default: return Keyboard::Scancode::Unknown;
	}
}
//-----------------------------------------------------------------------------
void WindowSystem::setTracking(bool track)
{
	TRACKMOUSEEVENT mouseEvent;
	mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
	mouseEvent.dwFlags = track ? TME_LEAVE : TME_CANCEL;
	mouseEvent.hwndTrack = m_handle;
	mouseEvent.dwHoverTime = HOVER_DEFAULT;
	TrackMouseEvent(&mouseEvent);
}
//-----------------------------------------------------------------------------