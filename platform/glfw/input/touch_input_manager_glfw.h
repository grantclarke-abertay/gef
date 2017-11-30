#ifndef _GEF_TOUCH_INPUT_MANAGER_GLFW_H
#define _GEF_TOUCH_INPUT_MANAGER_GLFW_H

#include <input/touch_input_manager.h>

namespace gef
{
    class PlatformGLFW;

    class TouchInputManagerGLFW : public TouchInputManager
    {
    public:
        TouchInputManagerGLFW(const PlatformGLFW* platform);
        ~TouchInputManagerGLFW();

        void Update();

        void EnablePanel(const Int32 panel_index);
        void DisablePanel(const Int32 panel_index);
        const Touch& GetTouch(const Int32 panel_index, const Int32 touch_index) { return touches_[panel_index][touch_index]; }
        const Int32 max_num_touches() const { return 2; }
        const Int32 max_num_panels() const { return 1; }
        const bool panel_enabled(const Int32 panel_index) const { return panel_enabled_[panel_index]; }

        const gef::Vector2& mouse_position() const override { return mouse_position_; }
        const gef::Vector4& mouse_rel() const override { return mouse_rel_; }
        bool is_button_down(Int32 button_num) const override { return is_button_down_[button_num]; }

        void set_live_button_down(Int32 button_num, bool is_down) { is_live_button_down_[button_num] = is_down; }

    private:
        void CleanUp();
        const PlatformGLFW* platform_;
        Touch touches_[1][2];
        bool panel_enabled_[1];
        bool is_button_down_[4];
        bool is_live_button_down_[4];
        gef::Vector2 mouse_position_;
        gef::Vector4 mouse_rel_;
    };
}

#endif // _GEF_TOUCH_INPUT_MANAGER_GLFW_H