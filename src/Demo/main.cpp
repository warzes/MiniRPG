#include "stdafx.h"
#include "GameApp.h"
//-----------------------------------------------------------------------------
#if defined(_MSC_VER)
#	pragma comment( lib, "3rdparty.lib" )
#	pragma comment( lib, "Engine.lib" )

#	pragma comment( lib, "reactphysics3d.lib" )
#	pragma comment( lib, "jsoncpp_static.lib" )

#	pragma comment( lib, "flac.lib" )
#	pragma comment( lib, "ogg.lib" )
#	pragma comment( lib, "vorbis.lib" )
#	pragma comment( lib, "vorbisenc.lib" )
#	pragma comment( lib, "vorbisfile.lib" )

#	if defined(_DEBUG)
#		pragma comment( lib, "assimp-vc143-mtd.lib" )
#		pragma comment( lib, "zlibstaticd.lib" )
#	else
#		pragma comment( lib, "assimp-vc143-mt.lib" )
#		pragma comment( lib, "zlibstatic.lib" )
#	endif
#endif
//-----------------------------------------------------------------------------
int main(
	[[maybe_unused]] int   argc,
	[[maybe_unused]] char* argv[])
{
	GameApp app;
	app.Run();
}
//-----------------------------------------------------------------------------