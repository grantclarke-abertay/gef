#include <platform/win32/system/window_win32.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
	default:
		{
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}

namespace gef
{

WindowWin32::WindowWin32(HINSTANCE hinstance, UInt32 screen_width, UInt32 screen_height, bool fullscreen) :
hwnd_(NULL)
{
	DEVMODE screen_settings;
	UInt32 wnd_pos_x, wnd_pos_y;

	bits_per_pixel_ = 32;

	// Get the instance of this application.
	hinstance_ = hinstance;

	// Give the application a name.
	application_name_ = "Framework Application";

	// Setup the windows class with default settings.
	WNDCLASSEX wc;
	memset(&wc, 0, sizeof(wc));
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hinstance_;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = application_name_;
	wc.cbSize        = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);


	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	fullscreen_ = fullscreen;
	if(fullscreen_)
	{
		// Determine the resolution of the clients desktop screen.
		screen_width  = GetSystemMetrics(SM_CXSCREEN);
		screen_height = GetSystemMetrics(SM_CYSCREEN);

		width_ = screen_width;
		height_ = screen_height;

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&screen_settings, 0, sizeof(screen_settings));
		screen_settings.dmSize       = sizeof(screen_settings);
		screen_settings.dmPelsWidth  = (unsigned long)screen_width;
		screen_settings.dmPelsHeight = (unsigned long)screen_height;
		screen_settings.dmBitsPerPel = bits_per_pixel_;			
		screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&screen_settings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		wnd_pos_x = wnd_pos_y = 0;
	}
	else
	{
		width_ = screen_width;
		height_ = screen_height;

		screen_width  += (UInt32)GetSystemMetrics(SM_CXSIZEFRAME)*2;
		screen_height += (UInt32)GetSystemMetrics(SM_CXSIZEFRAME)*2+(UInt32)GetSystemMetrics(SM_CYCAPTION);

		// Place the window in the middle of the screen.
		if((UInt32)GetSystemMetrics(SM_CXSCREEN) < screen_width)
			wnd_pos_x = 0;
		else
			wnd_pos_x = (UInt32)(GetSystemMetrics(SM_CXSCREEN) - screen_width) >> 1;
		if((UInt32)GetSystemMetrics(SM_CYSCREEN) < screen_height)
			wnd_pos_y = 0;
		else
			wnd_pos_y = (UInt32)(GetSystemMetrics(SM_CYSCREEN) - screen_height) >> 1;
	}

	// Create the window with the screen settings and get the handle to it.
	if(fullscreen_)
	{
		hwnd_ = CreateWindowEx(WS_EX_TOPMOST, application_name_, application_name_, 
			WS_POPUP,
			wnd_pos_x, wnd_pos_y, screen_width, screen_height, NULL, NULL, hinstance_, NULL);
	}
	else
	{
		hwnd_ = CreateWindowEx(WS_EX_APPWINDOW, application_name_, application_name_, 
			WS_OVERLAPPEDWINDOW,
			wnd_pos_x, wnd_pos_y, screen_width, screen_height, NULL, NULL, hinstance_, NULL);
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(hwnd_, SW_SHOW);
	SetForegroundWindow(hwnd_);
	SetFocus(hwnd_);

	// Hide the mouse cursor.
//	ShowCursor(false);
}

WindowWin32::~WindowWin32()
{
	// Show the mouse cursor.
//	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(fullscreen_)
		ChangeDisplaySettings(NULL, 0);

	// Remove the window.
	DestroyWindow(hwnd_);
	hwnd_ = NULL;

	// Remove the application instance.
	UnregisterClass(application_name_, hinstance_);
	hinstance_ = NULL;
}

}