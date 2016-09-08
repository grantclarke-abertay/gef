#include <platform/d3d11/system/platform_d3d11.h>
#include "scene_app.h"

unsigned int sceLibcHeapSize = 128*1024*1024;	// Sets up the heap area size as 128MiB.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	// initialisation
	gef::PlatformD3D11 platform(hInstance, 960, 544, false, true);

	SceneApp myApp(platform);
	myApp.Run();

	return 0;
}