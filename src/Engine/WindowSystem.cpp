#include "stdafx.h"
#include "Core.h"
#include "WindowSystem.h"
#include "LogSystem.h"
//-----------------------------------------------------------------------------
#pragma comment(lib, "opengl32.lib")
//-----------------------------------------------------------------------------
constexpr const wchar_t* TempWndClassName = L"TemporaryWindowMiniEngine";
constexpr const wchar_t* WndClassName = L"MainWindowMiniEngine";
//-----------------------------------------------------------------------------
struct TemporaryWindow final
{
	TemporaryWindow(LogSystem &log, HINSTANCE hInstance);
	~TemporaryWindow();

	HINSTANCE hInstance = nullptr;
	HWND hwnd = nullptr;
	HDC dc = nullptr;
	HGLRC glContext = nullptr;

	bool success = false;
};
//-----------------------------------------------------------------------------
TemporaryWindow::TemporaryWindow(LogSystem& log, HINSTANCE hInstance)
{
	this->hInstance = hInstance;

	WNDCLASSEX wc2{};
	wc2.cbSize = sizeof(WNDCLASSEX);
	wc2.style = CS_OWNDC;
	wc2.lpfnWndProc = &DefWindowProc;
	wc2.hInstance = hInstance;
	wc2.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc2.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc2.lpszClassName = TempWndClassName;
	ATOM temporaryWindowClass = RegisterClassEx(&wc2);
	if (!temporaryWindowClass)
	{
		log.Fatal("Win32: Failed to register temporary window class");
		return;
	}

	hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, MAKEINTATOM(temporaryWindowClass), TempWndClassName, WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 1, 1, nullptr, nullptr, hInstance, nullptr);
	if (!hwnd)
	{
		log.Fatal("Win32: Failed to create temporary window");
		return;
	}
	ShowWindow(hwnd, SW_HIDE);

	MSG msg{};
	while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	dc = GetDC(hwnd);
	if (!dc)
	{
		log.Fatal("Windows failed to provide a display context!");
		return;
	}
	// Please note, a valid correct PFD here will make the loading process faster by like 200-500ms
	PIXELFORMATDESCRIPTOR pfd{};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelfmt = ChoosePixelFormat(dc, &pfd);
	if (pixelfmt == 0)
	{
		log.Fatal("WGL: Failed to choose pixel format for dummy context");
		return;
	}

	if (!SetPixelFormat(dc, pixelfmt, &pfd))
	{
		log.Fatal("WGL: Failed to set pixel format for dummy context");
		return;
	}

	glContext = wglCreateContext(dc);
	if (!glContext)
	{
		log.Fatal("WGL: Failed to create dummy context");
		return;
	}

	if (!wglMakeCurrent(dc, glContext))
	{
		log.Fatal("WGL: Failed to make dummy context current");
		return;
	}

	if (!gladLoaderLoadWGL(dc))
	{
		log.Fatal("GLAD: Failed load WGL Func");
		return;
	}

	success = true;
}
//-----------------------------------------------------------------------------
TemporaryWindow::~TemporaryWindow()
{
	if (glContext && dc) wglMakeCurrent(nullptr, nullptr);
	if (glContext) wglDeleteContext(glContext);
	if (dc) ReleaseDC(hwnd, dc);
	if (hwnd) DestroyWindow(hwnd);
	UnregisterClass(TempWndClassName, hInstance);
}
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (message)
	{
	case WM_CREATE:
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//-----------------------------------------------------------------------------
WindowSystem::WindowSystem(Systems& systems)
	: m_systems(systems)
{
	m_systems.window = this;
}
//-----------------------------------------------------------------------------
WindowSystem::~WindowSystem()
{
	Destroy();
}
//-----------------------------------------------------------------------------
bool WindowSystem::Create(const WindowSystemCreateInfo& createInfo)
{
	assert(m_systems.log);

	assert(!m_hInstance);
	m_hInstance = GetModuleHandle(nullptr);

	TemporaryWindow* tempWindow = new TemporaryWindow(*m_systems.log, m_hInstance);
	bool tempWndSuccess = tempWindow->success;
	delete tempWindow;
	if (tempWndSuccess == false)
	{
		m_systems.log->Fatal("WindowSystem: Failed to create temporary window");
		return false;
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

	m_hwnd = CreateWindowEx(0, WndClassName, createInfo.title, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		nullptr, nullptr, m_hInstance, nullptr);
	if (!m_hwnd)
	{
		m_systems.log->Fatal("Windows failed to create window");
		return false;
	}

	// Center on screen
	{		
		uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - windowRect.right) / 2;
		uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - windowRect.bottom) / 2;
		SetWindowPos(m_hwnd, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	}

	ShowWindow(m_hwnd, SW_SHOW);

	GetClientRect(m_hwnd, &windowRect);
	m_frameWidth = windowRect.right - windowRect.left;
	m_frameHeight = windowRect.bottom - windowRect.top;

	m_deviceContext = GetDC(m_hwnd);
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
	const int attribs[] = 
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 6,
		WGL_CONTEXT_LAYER_PLANE_ARB, 0, // main plane
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	m_glContext = wglCreateContextAttribsARB(m_deviceContext, nullptr, attribs);
	if (m_glContext == nullptr)
	{
		m_systems.log->Fatal("2 Could not create wgl context!");
		return false;
	}

	wglMakeCurrent(m_deviceContext, m_glContext);

	int gladSuccess = gladLoaderLoadGL();
	gladLoaderUnloadGL();
	if (!gladSuccess)
	{
		m_systems.log->Fatal("glad: : Failed load gl Func");
		return false;
	}

	m_shouldQuit = false;

	return true;
}
//-----------------------------------------------------------------------------
void WindowSystem::Destroy()
{
	if (m_glContext && m_deviceContext) wglMakeCurrent(nullptr, nullptr);
	if (m_glContext) wglDeleteContext(m_glContext);
	if (m_deviceContext) ReleaseDC(m_hwnd, m_deviceContext);
	if (m_hwnd) DestroyWindow(m_hwnd);
	if (m_hInstance) UnregisterClass(WndClassName, m_hInstance);

	m_glContext = nullptr;
	m_deviceContext = nullptr;
	m_hwnd = nullptr;
	m_hInstance = nullptr;
	m_systems.window = nullptr;
}
//-----------------------------------------------------------------------------
bool WindowSystem::PeekMessages()
{
	if (PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
		if (m_msg.message == WM_QUIT)
			m_shouldQuit = true;

		return true;
	}
	return false;
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
	return m_hwnd;
}
//-----------------------------------------------------------------------------