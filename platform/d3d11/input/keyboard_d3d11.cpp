#include "keyboard_d3d11.h"
#include <gef.h>
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	static unsigned char dik_codes[Keyboard::NUM_KEY_CODES]=
	{
		DIK_0, //KC_0 = 0,
		DIK_1,	   //	KC_1,
		DIK_2,	   //	KC_2,
		DIK_3,	   //	KC_3,
		DIK_4,	   //	KC_4,
		DIK_5,	   //	KC_5,
		DIK_6,	   //	KC_6,
		DIK_7,	   //	KC_7,
		DIK_8,	   //	KC_8,
		DIK_9,	   //	KC_9,
		DIK_A,	   //	KC_A,
		DIK_B,	   //	KC_B,
		DIK_C,	   //	KC_C,
		DIK_D,	   //	KC_D,
		DIK_E,	   //	KC_E,
		DIK_F,	   //	KC_F,
		DIK_G,	   //	KC_G,
		DIK_H,	   //	KC_H,
		DIK_I,	   //	KC_I,
		DIK_J,	   //	KC_J,
		DIK_K,	   //	KC_K,
		DIK_L,	   //	KC_L,
		DIK_M,	   //	KC_M,
		DIK_N,	   //	KC_N,
		DIK_O,	   //	KC_O,
		DIK_P,	   //	KC_P,
		DIK_Q,	   //	KC_Q,
		DIK_R,	   //	KC_R,
		DIK_S,	   //	KC_S,
		DIK_T,	   //	KC_T,
		DIK_U,	   //	KC_U,
		DIK_V,	   //	KC_V,
		DIK_W,	   //	KC_W,
		DIK_X,	   //	KC_X,
		DIK_Y,	   //	KC_Y,
		DIK_Z,	   //	KC_Z,
		DIK_F1,	   //	KC_F1,
		DIK_F2,	   //	KC_F2,
		DIK_F3,	   //	KC_F3,
		DIK_F4,	   //	KC_F4,
		DIK_F5,	   //	KC_F5,
		DIK_F6,	   //	KC_F6,
		DIK_F7,	   //	KC_F7,
		DIK_F8,	   //	KC_F8,
		DIK_F9,	   //	KC_F9,
		DIK_F10,	   //	KC_F10,
		DIK_F11,	   //	KC_F11,
		DIK_F12,	   //	KC_F12,
		DIK_F13,	   //	KC_F13,
		DIK_F14,	   //	KC_F14,
		DIK_F15,	   //	KC_F15,
		DIK_NUMPAD0,   //	KC_NUMPAD0,
		DIK_NUMPAD1,   //	KC_NUMPAD1,
		DIK_NUMPAD2,   //	KC_NUMPAD2,
		DIK_NUMPAD3,   //	KC_NUMPAD3,
		DIK_NUMPAD4,   //	KC_NUMPAD4,
		DIK_NUMPAD5,   //	KC_NUMPAD5,
		DIK_NUMPAD6,   //	KC_NUMPAD6,
		DIK_NUMPAD7,   //	KC_NUMPAD7,
		DIK_NUMPAD8,   //	KC_NUMPAD8,
		DIK_NUMPAD9,   //	KC_NUMPAD9,
		DIK_NUMPADENTER,   //	KC_NUMPADENTER,
		DIK_NUMPADSTAR,   //	KC_NUMPADSTAR,
		DIK_NUMPADEQUALS,   //	KC_NUMPADEQUALS,
		DIK_NUMPADMINUS,   //	KC_NUMPADMINUS,
		DIK_NUMPADPLUS,   //	KC_NUMPADPLUS,
		DIK_NUMPADPERIOD,   //	KC_NUMPADPERIOD,
		DIK_NUMPADSLASH,   //	KC_NUMPADSLASH,
		DIK_ESCAPE,   //	KC_ESCAPE,
		DIK_TAB,   //	KC_TAB,
		DIK_LSHIFT,   //	KC_LSHIFT,
		DIK_RSHIFT,   //	KC_RSHIFT,
		DIK_LCONTROL,   //	KC_LCONTROL,
		DIK_RCONTROL,   //	KC_RCONTROL,
		DIK_BACKSPACE,   //	KC_BACKSPACE,
		DIK_RETURN,   //	KC_RETURN,
		DIK_LALT,   //	KC_LALT,
		DIK_SPACE,   //	KC_SPACE,
		DIK_CAPSLOCK,   //	KC_CAPSLOCK,
		DIK_NUMLOCK,   //	KC_NUMLOCK,
		DIK_SCROLL,   //	KC_SCROLL,
		DIK_RALT,   //	KC_RALT,
		DIK_AT,   //	KC_AT,
		DIK_COLON,   //	KC_COLON,
		DIK_UNDERLINE,   //	KC_UNDERLINE,
		DIK_MINUS,   //	KC_MINUS,
		DIK_EQUALS,   //	KC_EQUALS,
		DIK_LBRACKET,   //	KC_LBRACKET,
		DIK_RBRACKET,   //	KC_RBRACKET,
		DIK_SEMICOLON,   //	KC_SEMICOLON,
		DIK_APOSTROPHE,   //	KC_APOSTROPHE,
		DIK_GRAVE,   //	KC_GRAVE,
		DIK_BACKSLASH,   //	KC_BACKSLASH,
		DIK_COMMA,   //	KC_COMMA,
		DIK_PERIOD,   //	KC_PERIOD,
		DIK_SLASH,	//	KC_SLASH,
		DIK_UP,		// KC_UP
		DIK_DOWN,	// KC_DOWN
		DIK_LEFT,	// KC_LEFT
		DIK_RIGHT,	// KC_RIGHT
			DIK_PGUP, // KC_PGUP
			DIK_PGDN, // KC_PGDN
	};

	KeyboardD3D11::KeyboardD3D11(const PlatformD3D11& platform, LPDIRECTINPUT8 direct_input)
		: direct_input_(direct_input)
		, keyboard_(NULL)
	{
		HRESULT result = S_OK;

		ResetKeyboardStates();

		// Initialize the direct input interface for the keyboard.
		result = direct_input_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
		if (SUCCEEDED(result))
		{
			// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
			result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
		}

		// Set the cooperative level of the keyboard to not share with other programs.
		if (SUCCEEDED(result))
		{
			result = keyboard_->SetCooperativeLevel(platform.top_level_hwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			// Now acquire the keyboard.
			result = keyboard_->Acquire();
		}

		if (FAILED(result))
			CleanUp();
	}

	KeyboardD3D11::~KeyboardD3D11()
	{
		CleanUp();
	}

	void KeyboardD3D11::CleanUp()
	{
		if (keyboard_)
		{
			keyboard_->Unacquire();
			keyboard_->Release();
			keyboard_ = NULL;
		}
	}

	void KeyboardD3D11::ResetKeyboardStates()
	{
		memset(live_keyboard_state_, 0, 256);
//		memset(current_update_keyboard_state_, 0, 256);
		memset(previous_update_keyboard_state_, 0, 256);
	}


	void KeyboardD3D11::Update()
	{
		if (keyboard_)
		{
			HRESULT result = S_OK;

			memcpy_s(previous_update_keyboard_state_, sizeof(previous_update_keyboard_state_), live_keyboard_state_, sizeof(live_keyboard_state_));

			// Read the keyboard device.
			result = keyboard_->GetDeviceState(sizeof(live_keyboard_state_), (LPVOID)&live_keyboard_state_);
			if (FAILED(result))
			{
				// If the keyboard lost focus or was not acquired then try to get control back.
				if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
				{
					keyboard_->Acquire();
					ResetKeyboardStates();
				}

				// something has gone wrong, reset keyboard states so no false inputs are read
				ResetKeyboardStates();
			}
//			else
//			{
//				memcpy_s(current_update_keyboard_state_, sizeof(current_update_keyboard_state_), live_keyboard_state_, sizeof(live_keyboard_state_));
//			}
		}
	}

	bool KeyboardD3D11::IsKeyDown(KeyCode key) const
	{
		int dik_code = dik_codes[key];
		bool key_down = (live_keyboard_state_[dik_code] & 0x80) ? true : false;
		return key_down;
	}

	bool KeyboardD3D11::IsKeyPressed(KeyCode key) const
	{
		int dik_code = dik_codes[key];
		bool previous_key_down = (previous_update_keyboard_state_[dik_code] & 0x80) ? true : false;
		bool key_down = (live_keyboard_state_[dik_code] & 0x80) ? true : false;
		return !previous_key_down && key_down;
	}

	bool KeyboardD3D11::IsKeyReleased(KeyCode key) const
	{
		int dik_code = dik_codes[key];
		bool previous_key_down = (previous_update_keyboard_state_[dik_code] & 0x80) ? true : false;
		bool key_down = (live_keyboard_state_[dik_code] & 0x80) ? true : false;
		return previous_key_down && !key_down;
	}
}
