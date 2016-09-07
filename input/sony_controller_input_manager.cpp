#include <input/sony_controller_input_manager.h>

namespace gef
{
	SonyControllerInputManager::SonyControllerInputManager(const Platform& platform)
	{
	}

	SonyControllerInputManager::~SonyControllerInputManager()
	{
	}

	SonyController::SonyController() :
		buttons_down_(0),
		buttons_pressed_(0),
		buttons_released_(0),
		left_stick_x_axis_(0.0f),
		left_stick_y_axis_(0.0f),
		right_stick_x_axis_(0.0f),
		right_stick_y_axis_(0.0f)
	{
	}

	void SonyController::UpdateButtonStates(UInt32 previous_buttons_down)
	{
		// flag buttons that have been pressed this update
		buttons_pressed_  = buttons_down_ & ~previous_buttons_down;

		// flag buttons that have been released this update
		buttons_released_ = ~buttons_down_ & previous_buttons_down;
	}
}