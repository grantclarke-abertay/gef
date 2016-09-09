#ifndef _PLATFORM_D3D11_INPUT_KEYBOARD_H
#define _PLATFORM_D3D11_INPUT_KEYBOARD_H

#include <input/keyboard.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dinputd.h>

namespace gef
{
	class KeyboardD3D11 : public Keyboard
	{
	public:
		KeyboardD3D11(LPDIRECTINPUT8 direct_input);
		~KeyboardD3D11();
		void Update();
		bool IsKeyDown(KeyCode key) const;
		bool IsKeyPressed(KeyCode key) const;
		bool IsKeyReleased(KeyCode key) const;
	protected:
		LPDIRECTINPUT8 direct_input_;
		IDirectInputDevice8* keyboard_;

		unsigned char previous_keyboard_state_[256];
		unsigned char keyboard_state_[256];

		void CleanUp();
		void ResetKeyboardStates();
	};
}

#endif // !_PLATFORM_D3D11_INPUT_KEYBOARD_H
