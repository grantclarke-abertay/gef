#ifndef _PLATFORM_GLFW_INPUT_KEYBOARD_H
#define _PLATFORM_GLFW_INPUT_KEYBOARD_H

#include <input/keyboard.h>


namespace gef
{
    class PlatformGLFW;

    class KeyboardGLFW : public Keyboard
    {
    public:
        KeyboardGLFW(const PlatformGLFW& platform);
        ~KeyboardGLFW();
        void Update();
        bool IsKeyDown(KeyCode key) const;
        bool IsKeyPressed(KeyCode key) const;
        bool IsKeyReleased(KeyCode key) const;
    protected:
        unsigned char previous_update_keyboard_state_[256];
        //unsigned char current_update_keyboard_state_[256];
        unsigned char live_keyboard_state_[256];


        const PlatformGLFW& platform_;

        void CleanUp();
        void ResetKeyboardStates();



    };
}

#endif // !_PLATFORM_GLFW_INPUT_KEYBOARD_H