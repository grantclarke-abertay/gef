#include "keyboard_glfw.h"
#include <gef.h>
#include <platform/glfw/system/platform_glfw.h>
#include <cstring>

namespace gef
{
    static int glfw_key_codes[Keyboard::NUM_KEY_CODES]=
            {
                    GLFW_KEY_0, //KC_0 = 0,
                    GLFW_KEY_1,	   //	KC_1,
                    GLFW_KEY_2,	   //	KC_2,
                    GLFW_KEY_3,	   //	KC_3,
                    GLFW_KEY_4,	   //	KC_4,
                    GLFW_KEY_5,	   //	KC_5,
                    GLFW_KEY_6,	   //	KC_6,
                    GLFW_KEY_7,	   //	KC_7,
                    GLFW_KEY_8,	   //	KC_8,
                    GLFW_KEY_9,	   //	KC_9,
                    GLFW_KEY_A,	   //	KC_A,
                    GLFW_KEY_B,	   //	KC_B,
                    GLFW_KEY_C,	   //	KC_C,
                    GLFW_KEY_D,	   //	KC_D,
                    GLFW_KEY_E,	   //	KC_E,
                    GLFW_KEY_F,	   //	KC_F,
                    GLFW_KEY_G,	   //	KC_G,
                    GLFW_KEY_H,	   //	KC_H,
                    GLFW_KEY_I,	   //	KC_I,
                    GLFW_KEY_J,	   //	KC_J,
                    GLFW_KEY_K,	   //	KC_K,
                    GLFW_KEY_L,	   //	KC_L,
                    GLFW_KEY_M,	   //	KC_M,
                    GLFW_KEY_N,	   //	KC_N,
                    GLFW_KEY_O,	   //	KC_O,
                    GLFW_KEY_P,	   //	KC_P,
                    GLFW_KEY_Q,	   //	KC_Q,
                    GLFW_KEY_R,	   //	KC_R,
                    GLFW_KEY_S,	   //	KC_S,
                    GLFW_KEY_T,	   //	KC_T,
                    GLFW_KEY_U,	   //	KC_U,
                    GLFW_KEY_V,	   //	KC_V,
                    GLFW_KEY_W,	   //	KC_W,
                    GLFW_KEY_X,	   //	KC_X,
                    GLFW_KEY_Y,	   //	KC_Y,
                    GLFW_KEY_Z,	   //	KC_Z,
                    GLFW_KEY_F1,	   //	KC_F1,
                    GLFW_KEY_F2,	   //	KC_F2,
                    GLFW_KEY_F3,	   //	KC_F3,
                    GLFW_KEY_F4,	   //	KC_F4,
                    GLFW_KEY_F5,	   //	KC_F5,
                    GLFW_KEY_F6,	   //	KC_F6,
                    GLFW_KEY_F7,	   //	KC_F7,
                    GLFW_KEY_F8,	   //	KC_F8,
                    GLFW_KEY_F9,	   //	KC_F9,
                    GLFW_KEY_F10,	   //	KC_F10,
                    GLFW_KEY_F11,	   //	KC_F11,
                    GLFW_KEY_F12,	   //	KC_F12,
                    GLFW_KEY_F13,	   //	KC_F13,
                    GLFW_KEY_F14,	   //	KC_F14,
                    GLFW_KEY_F15,	   //	KC_F15,
                    GLFW_KEY_KP_0,   //	KC_KP_0,
                    GLFW_KEY_KP_1,   //	KC_KP_1,
                    GLFW_KEY_KP_2,   //	KC_KP_2,
                    GLFW_KEY_KP_3,   //	KC_KP_3,
                    GLFW_KEY_KP_4,   //	KC_KP_4,
                    GLFW_KEY_KP_5,   //	KC_KP_5,
                    GLFW_KEY_KP_6,   //	KC_KP_6,
                    GLFW_KEY_KP_7,   //	KC_KP_7,
                    GLFW_KEY_KP_8,   //	KC_KP_8,
                    GLFW_KEY_KP_9,   //	KC_KP_9,
                    GLFW_KEY_KP_ENTER,   //	KC_KP_ENTER,
                    GLFW_KEY_KP_MULTIPLY,   //	KC_KP_STAR,
                    GLFW_KEY_KP_EQUAL,   //	KC_KP_EQUALS,
                    GLFW_KEY_KP_SUBTRACT,   //	KC_KP_MINUS,
                    GLFW_KEY_KP_ADD,   //	KC_KP_PLUS,
                    GLFW_KEY_KP_DECIMAL,   //	KC_KP_PERIOD,
                    GLFW_KEY_KP_DIVIDE,   //	KC_KP_SLASH,
                    GLFW_KEY_ESCAPE,   //	KC_ESCAPE,
                    GLFW_KEY_TAB,   //	KC_TAB,
                    GLFW_KEY_LEFT_SHIFT,   //	KC_LSHIFT,
                    GLFW_KEY_RIGHT_SHIFT,   //	KC_RSHIFT,
                    GLFW_KEY_LEFT_CONTROL,   //	KC_LCONTROL,
                    GLFW_KEY_RIGHT_CONTROL,   //	KC_RCONTROL,
                    GLFW_KEY_BACKSPACE,   //	KC_BACKSPACE,
                    GLFW_KEY_ENTER,   //	KC_RETURN,
                    GLFW_KEY_LEFT_ALT,   //	KC_LALT,
                    GLFW_KEY_SPACE,   //	KC_SPACE,
                    GLFW_KEY_CAPS_LOCK,   //	KC_CAPSLOCK,
                    GLFW_KEY_NUM_LOCK,   //	KC_NUMLOCK,
                    GLFW_KEY_SCROLL_LOCK,   //	KC_SCROLL,
                    GLFW_KEY_RIGHT_ALT,   //	KC_RALT,
                    GLFW_KEY_APOSTROPHE,   //	KC_AT,
                    GLFW_KEY_SEMICOLON,   //	KC_COLON,
                    GLFW_KEY_UNKNOWN,   //	KC_UNDERLINE,
                    GLFW_KEY_MINUS,   //	KC_MINUS,
                    GLFW_KEY_EQUAL,   //	KC_EQUALS,
                    GLFW_KEY_LEFT_BRACKET,   //	KC_LBRACKET,
                    GLFW_KEY_RIGHT_BRACKET,   //	KC_RBRACKET,
                    GLFW_KEY_SEMICOLON,   //	KC_SEMICOLON,
                    GLFW_KEY_APOSTROPHE,   //	KC_APOSTROPHE,
                    GLFW_KEY_GRAVE_ACCENT,   //	KC_GRAVE,
                    GLFW_KEY_BACKSLASH,   //	KC_BACKSLASH,
                    GLFW_KEY_COMMA,   //	KC_COMMA,
                    GLFW_KEY_PERIOD,   //	KC_PERIOD,
                    GLFW_KEY_SLASH,	//	KC_SLASH,
                    GLFW_KEY_UP,		// KC_UP
                    GLFW_KEY_DOWN,	// KC_DOWN
                    GLFW_KEY_LEFT,	// KC_LEFT
                    GLFW_KEY_RIGHT,	// KC_RIGHT
                    GLFW_KEY_PAGE_UP, // KC_PGUP
                    GLFW_KEY_PAGE_DOWN, // KC_PGDN
            };

    KeyboardGLFW::KeyboardGLFW(const PlatformGLFW& platform) :
        platform_(platform)
    {
        ResetKeyboardStates();
    }

    KeyboardGLFW::~KeyboardGLFW()
    {
        CleanUp();
    }

    void KeyboardGLFW::CleanUp()
    {

    }

    void KeyboardGLFW::ResetKeyboardStates()
    {
        memset(live_keyboard_state_, 0, 256);
//		memset(current_update_keyboard_state_, 0, 256);
        memset(previous_update_keyboard_state_, 0, 256);
    }


    void KeyboardGLFW::Update()
    {
        memcpy(previous_update_keyboard_state_, live_keyboard_state_, sizeof(live_keyboard_state_));


        for(int key_num=0;key_num<Keyboard::NUM_KEY_CODES;++key_num)
        {
            if(glfw_key_codes[key_num] != GLFW_KEY_UNKNOWN)
            {
                int key_state = glfwGetKey(platform_.window(), glfw_key_codes[key_num]);

                if(key_state == GLFW_PRESS)
                    live_keyboard_state_[key_num] = 0x80;
                else if(key_state == GLFW_RELEASE)
                    live_keyboard_state_[key_num] = 0x00;
            }
        }
    }

    bool KeyboardGLFW::IsKeyDown(KeyCode key) const
    {
        bool key_down = (live_keyboard_state_[key] & 0x80) ? true : false;
        return key_down;
    }

    bool KeyboardGLFW::IsKeyPressed(KeyCode key) const
    {
        bool previous_key_down = (previous_update_keyboard_state_[key] & 0x80) ? true : false;
        bool key_down = (live_keyboard_state_[key] & 0x80) ? true : false;
        return !previous_key_down && key_down;
    }

    bool KeyboardGLFW::IsKeyReleased(KeyCode key) const
    {
        bool previous_key_down = (previous_update_keyboard_state_[key] & 0x80) ? true : false;
        bool key_down = (live_keyboard_state_[key] & 0x80) ? true : false;
        return previous_key_down && !key_down;
    }
}
