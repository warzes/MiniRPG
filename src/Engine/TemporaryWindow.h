#pragma once
constexpr const wchar_t* TempWndClassName = L"TemporaryWindowMiniEngine";
//-----------------------------------------------------------------------------
struct TemporaryWindow final
{
	TemporaryWindow(LogSystem& log, HINSTANCE hInstance);
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