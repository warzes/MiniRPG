#include "stdafx.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "Engine.lib" )
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	Systems systems;
	LogSystem log(systems);
	WindowSystem window(systems);

	window.Create({});

	while (1)
	{
		if (window.ShouldQuit()) break;
		if (window.PeekMessages())
			continue;

		glClearColor(0.2f, 0.4f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		window.SwapBuffers();
	}

	window.Destroy();
}
//-----------------------------------------------------------------------------