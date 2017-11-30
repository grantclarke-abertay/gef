//
// Created by grant on 23/11/17.
//

#include "input_manager_glfw.h"
#include "touch_input_manager_glfw.h"
#include "keyboard_glfw.h"
#include <platform/glfw/system/platform_glfw.h>

namespace gef
{
    InputManager* InputManager::Create(Platform& platform)
    {
        return new InputManagerGLFW(platform);
    }

    InputManagerGLFW::InputManagerGLFW(Platform& platform)
            : InputManager(platform)
            , platform_(platform)
    {
        PlatformGLFW& platform_d3d11 = reinterpret_cast<PlatformGLFW&>(platform);

        touch_manager_ = new TouchInputManagerGLFW(&platform_d3d11);
        platform.set_touch_input_manager(touch_manager_);
        keyboard_ = new KeyboardGLFW(platform_d3d11);
//        controller_manager_ = new SonyControllerInputManagerD3D11(platform_d3d11, direct_input_);
    }

    InputManagerGLFW::~InputManagerGLFW()
    {
        delete touch_manager_;
        platform_.set_touch_input_manager(NULL);

        delete keyboard_;
//        delete controller_manager_;
    }

}