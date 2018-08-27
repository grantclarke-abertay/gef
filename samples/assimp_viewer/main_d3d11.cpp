#include <iostream>
#include <windows.h>
#include "assimp_viewer.h"
#include <platform/d3d11/system/platform_d3d11.h>
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


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	gef::PlatformD3D11 platform(hInstance, 1280, 720, false, true);

	WNDPROC prevWndProc = (WNDPROC)SetWindowLongPtr(platform.hwnd(), GWL_WNDPROC, (LONG_PTR)&myWndProc);


    ImGui_ImplDX11_Init(platform.hwnd(), platform.device(), platform.device_context());


    AssimpViewer myApplication(platform);
	myApplication.set_camera_key(gef::Keyboard::KC_LALT);

    myApplication.Run();

    ImGui_ImplDX11_Shutdown();
    return 0;
}
