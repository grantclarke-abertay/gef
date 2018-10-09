#ifndef _GEF_TOUCH_INPUT_MANAGER_D3D11_H
#define _GEF_TOUCH_INPUT_MANAGER_D3D11_H

#include <input/touch_input_manager.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace gef
{
	class PlatformD3D11;

	class TouchInputManagerD3D11 : public TouchInputManager
	{
	public:
		TouchInputManagerD3D11(const PlatformD3D11* platform, LPDIRECTINPUT8 direct_input);
		~TouchInputManagerD3D11();

		void Update();

		void EnablePanel(const Int32 panel_index);
		void DisablePanel(const Int32 panel_index);
		const Touch& GetTouch(const Int32 panel_index, const Int32 touch_index) { return touches_[panel_index][touch_index]; }
		const Int32 max_num_touches() const { return 2; }
		const Int32 max_num_panels() const { return 1; }
		const bool panel_enabled(const Int32 panel_index) const { return panel_enabled_[panel_index]; }

		inline const gef::Vector2 mouse_position() const { return mouse_position_; }
		inline const gef::Vector4 mouse_rel() const { return mouse_rel_; }
		inline bool is_button_down(Int32 button_num) const { return is_button_down_[button_num]; }

	private:
		void CleanUp();
		LPDIRECTINPUT8			direct_input_;
		LPDIRECTINPUTDEVICE8	mouse_;
		const PlatformD3D11* platform_;
		Touch touches_[1][2];
		bool panel_enabled_[1];
		bool is_button_down_[4];
		gef::Vector2 mouse_position_;
		gef::Vector4 mouse_rel_;
	};
}

#endif // _GEF_TOUCH_INPUT_MANAGER_D3D11_H
