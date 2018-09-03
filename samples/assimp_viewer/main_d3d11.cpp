#include <iostream>
#include <windows.h>
#include "assimp_viewer.h"
#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/win32/system/window_win32.h>
//

#include <imgui.h>
#include "imgui_impl_dx11.h"

void ImGui_NewFrame()
{
	ImGui_ImplDX11_NewFrame();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI myWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{

	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

#ifdef _CONSOLE
void InitializeConsoleStdIO()
{
	// si une console est rattachée au processus, alors il existe des fichiers
	// virtuel CONIN$ et CONOUT$ qui permettent respectivement de lire
	// et d'écrire depuis / dans cette console (voir la doc de CreateFile).

#if _MSC_VER >= 1400 // VC++ 8
	{
		// éviter le warning C4996: 'freopen' was declared deprecated
		// This function or variable may be unsafe. Consider using freopen_s instead.
		FILE *stream;
		freopen_s(&stream, "CONIN$", "r", stdin);
		freopen_s(&stream, "CONOUT$", "w", stdout);
		freopen_s(&stream, "CONOUT$", "w", stderr);
	}
#else
	std::freopen("CONIN$", "r", stdin);
	std::freopen("CONOUT$", "w", stdout);
	std::freopen("CONOUT$", "w", stderr);
#endif

	// la ligne suivante synchronise les flux standards C++ (cin, cout, cerr...)
	std::ios_base::sync_with_stdio();
}
#endif



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _CONSOLE
	::AllocConsole();
	InitializeConsoleStdIO();
#endif

	int width = 1280;
	int height = 720;
	bool fullscreen = false;
	bool vsyncenabled = true;

	gef::WindowWin32* window = new gef::WindowWin32(hInstance, width, height, fullscreen, myWndProc);


	// initialisation
	gef::PlatformD3D11 platform(hInstance, width, height, fullscreen, vsyncenabled, window->hwnd());

    ImGui_ImplDX11_Init(platform.hwnd(), platform.device(), platform.device_context());


    AssimpViewer myApplication(platform);
	myApplication.set_camera_key(gef::Keyboard::KC_LALT);

    myApplication.Run();



    ImGui_ImplDX11_Shutdown();

	delete window;
	window = NULL;

	FreeConsole();

    return 0;
}
