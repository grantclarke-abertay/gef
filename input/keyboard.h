#ifndef _GEF_INPUT_KEYBOARD_H
#define _GEF_INPUT_KEYBOARD_H

namespace gef
{

	class Keyboard
	{
	public:
		enum KeyCode
		{
			KC_0 = 0,
			KC_1,
			KC_2,
			KC_3,
			KC_4,
			KC_5,
			KC_6,
			KC_7,
			KC_8,
			KC_9,
			KC_A,
			KC_B,
			KC_C,
			KC_D,
			KC_E,
			KC_F,
			KC_G,
			KC_H,
			KC_I,
			KC_J,
			KC_K,
			KC_L,
			KC_M,
			KC_N,
			KC_O,
			KC_P,
			KC_Q,
			KC_R,
			KC_S,
			KC_T,
			KC_U,
			KC_V,
			KC_W,
			KC_X,
			KC_Y,
			KC_Z,
			KC_F1,
			KC_F2,
			KC_F3,
			KC_F4,
			KC_F5,
			KC_F6,
			KC_F7,
			KC_F8,
			KC_F9,
			KC_F10,
			KC_F11,
			KC_F12,
			KC_F13,
			KC_F14,
			KC_F15,
			KC_NUMPAD0,
			KC_NUMPAD1,
			KC_NUMPAD2,
			KC_NUMPAD3,
			KC_NUMPAD4,
			KC_NUMPAD5,
			KC_NUMPAD6,
			KC_NUMPAD7,
			KC_NUMPAD8,
			KC_NUMPAD9,
			KC_NUMPADENTER,
			KC_NUMPADSTAR,
			KC_NUMPADEQUALS,
			KC_NUMPADMINUS,
			KC_NUMPADPLUS,
			KC_NUMPADPERIOD,
			KC_NUMPADSLASH,
			KC_ESCAPE,
			KC_TAB,
			KC_LSHIFT,
			KC_RSHIFT,
			KC_LCONTROL,
			KC_RCONTROL,
			KC_BACKSPACE,
			KC_RETURN,
			KC_LALT,
			KC_SPACE,
			KC_CAPSLOCK,
			KC_NUMLOCK,
			KC_SCROLL,
			KC_RALT,
			KC_AT,
			KC_COLON,
			KC_UNDERLINE,
			KC_MINUS,
			KC_EQUALS,
			KC_LBRACKET,
			KC_RBRACKET,
			KC_SEMICOLON,
			KC_APOSTROPHE,
			KC_GRAVE,
			KC_BACKSLASH,
			KC_COMMA,
			KC_PERIOD,
			KC_SLASH,
			KC_UP,
			KC_DOWN,
			KC_LEFT,
			KC_RIGHT,
			KC_PGUP,
			KC_PGDN,
			NUM_KEY_CODES
		};


		virtual ~Keyboard();
		virtual void Update() = 0;

		virtual bool IsKeyDown(KeyCode key) const;
		virtual bool IsKeyPressed(KeyCode key) const;
		virtual bool IsKeyReleased(KeyCode key) const;
	};
}

#endif // _GEF_INPUT_KEYBOARD_H
