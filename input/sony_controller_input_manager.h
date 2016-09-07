#ifndef _GEF_SONY_CONTROLLER_INPUT_MANAGER_H
#define _GEF_SONY_CONTROLLER_INPUT_MANAGER_H

#include <gef.h>

#define gef_SONY_CTRL_SELECT				(1<<0)
#define gef_SONY_CTRL_L3					(1<<1)
#define gef_SONY_CTRL_R3					(1<<2)
#define gef_SONY_CTRL_START				(1<<3)
#define gef_SONY_CTRL_OPTIONS				(1<<3)
#define gef_SONY_CTRL_UP					(1<<4)
#define gef_SONY_CTRL_RIGHT				(1<<5)
#define gef_SONY_CTRL_DOWN					(1<<6)
#define gef_SONY_CTRL_LEFT					(1<<7)
#define gef_SONY_CTRL_L2					(1<<8)
#define gef_SONY_CTRL_R2					(1<<9)
#define gef_SONY_CTRL_L1					(1<<10)
#define gef_SONY_CTRL_R1					(1<<11)
#define gef_SONY_CTRL_TRIANGLE				(1<<12)
#define gef_SONY_CTRL_CIRCLE				(1<<13)
#define gef_SONY_CTRL_CROSS				(1<<14)
#define gef_SONY_CTRL_SQUARE				(1<<15)
#define gef_SONY_CTRL_TOUCH_PAD			(1<<20)

namespace gef
{
	class Platform;

	class SonyController
	{
	public:
		SonyController();
		void UpdateButtonStates(UInt32 previous_buttons_down);

		/// @return the buttons that are currently held down
		inline UInt32 buttons_down() const { return buttons_down_; }
		/// @return the buttons that have been pressed this update
		inline UInt32 buttons_pressed() const { return buttons_pressed_; }
		/// @return the buttons that have been released this update
		inline UInt32 buttons_released() const { return buttons_released_; }

		/// @return the horizontal position of the left analog stick [ -1 <-- left, 1 --> right]
		inline float left_stick_x_axis() const { return left_stick_x_axis_; }
		/// @return the vertical position of the left analog stick [ -1 <-- up, 1 --> down]
		inline float left_stick_y_axis() const { return left_stick_y_axis_; }
		/// @return the horizontal position of the right analog stick [ -1 <-- left, 1 --> right]
		inline float right_stick_x_axis() const { return right_stick_x_axis_; }
		/// @return the vertical position of the right analog stick [ -1 <-- up, 1 --> down]
		inline float right_stick_y_axis() const { return right_stick_y_axis_; }

		inline void set_buttons_down(const UInt32 buttons_down) { buttons_down_ = buttons_down; }
		inline void set_left_stick_x_axis(const float stick_value) { left_stick_x_axis_ = stick_value; }
		inline void set_left_stick_y_axis(const float stick_value) { left_stick_y_axis_ = stick_value; }
		inline void set_right_stick_x_axis(const float stick_value) { right_stick_x_axis_ = stick_value; }
		inline void set_right_stick_y_axis(const float stick_value) { right_stick_y_axis_ = stick_value; }


	private:
		UInt32 buttons_down_;
		UInt32 buttons_pressed_;		// Information for pressed buttons
		UInt32 buttons_released_;	// Information for released buttons
		UInt32 previous_buttons_down_;

		float left_stick_x_axis_;
		float left_stick_y_axis_;
		float right_stick_x_axis_;
		float right_stick_y_axis_;
	};

	class SonyControllerInputManager
	{
	public:
		SonyControllerInputManager(const Platform& platform);
		virtual ~SonyControllerInputManager();

		/// Update the controller data with the current status of the buttons and analog sticks
		///
		/// @return error status from reading the controllers [0 = success]
		virtual Int32 Update() = 0;
/*
		/// @return the buttons that are currently held down
		inline UInt32 buttons_down() const { return buttons_down_; }
		/// @return the buttons that have been pressed this update
		inline UInt32 buttons_pressed() const { return buttons_pressed_; }
		/// @return the buttons that have been released this update
		inline UInt32 buttons_released() const { return buttons_released_; }

		/// @return the horizontal position of the left analog stick [ -1 <-- left, 1 --> right]
		inline float left_stick_x_axis() const { return left_stick_x_axis_; }
		/// @return the vertical position of the left analog stick [ -1 <-- up, 1 --> down]
		inline float left_stick_y_axis() const { return left_stick_y_axis_; }
		/// @return the horizontal position of the right analog stick [ -1 <-- left, 1 --> right]
		inline float right_stick_x_axis() const { return right_stick_x_axis_; }
		/// @return the vertical position of the right analog stick [ -1 <-- up, 1 --> down]
		inline float right_stick_y_axis() const { return right_stick_y_axis_; }
*/
		inline const SonyController* const GetController(const Int32 controller_num) const { return controller_num == 0 ? &controller_ : 0; }


	protected:
		SonyController controller_;
/*
		void UpdateButtonStates();
		UInt32 buttons_down_;
		UInt32 buttons_pressed_;		// Information for pressed buttons
		UInt32 buttons_released_;	// Information for released buttons
		UInt32 previous_buttons_down_;

		float left_stick_x_axis_;
		float left_stick_y_axis_;
		float right_stick_x_axis_;
		float right_stick_y_axis_;

//		static float kStickDeadZone;
*/
	};

}

#endif // _GEF_SONY_CONTROLLER_INPUT_MANAGER_H
