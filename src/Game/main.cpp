#include "stdafx.h"
#include "TestApp.h"
#include "GameApp.h"
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
	//TestApp app;
	GameApp app;
	app.Run();
}
//-----------------------------------------------------------------------------