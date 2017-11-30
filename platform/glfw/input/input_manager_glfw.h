#ifndef _PLATFORM_GLFW_INPUT_INPUT_MANAGER_H
#define _PLATFORM_GLFW_INPUT_INPUT_MANAGER_H

#include <input/input_manager.h>


namespace gef
{
    class InputManagerGLFW : public InputManager
    {
    public:
        InputManagerGLFW(Platform& platform);
        ~InputManagerGLFW();
    protected:
        Platform& platform_;
    };
}
#endif // !_PLATFORM_GLFW_INPUT_INPUT_MANAGER_H