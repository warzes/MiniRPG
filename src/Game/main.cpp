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
	RenderSystem render(systems);

	window.Create({});
	render.Create({});

	while (1)
	{
		if (window.ShouldQuit()) break;
		if (window.PeekMessages())
			continue;

		render.BeginFrame();

		render.EndFrame();
	}

	render.Destroy();
	window.Destroy();
}
//-----------------------------------------------------------------------------