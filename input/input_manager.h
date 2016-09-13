#ifndef _GEF_INPUT_INPUT_MANAGER_H
#define _GEF_INPUT_INPUT_MANAGER_H

namespace gef
{
	class Platform;
	class TouchInputManager;
	class Keyboard;
	class SonyControllerInputManager;

	class InputManager
	{
	public:
		virtual ~InputManager();
		virtual void Update();

		inline TouchInputManager* touch_manager() const { return touch_manager_; }
		inline Keyboard* keyboard() const { return keyboard_; }
		inline SonyControllerInputManager* controller_input() const { return controller_manager_; }

		static InputManager* Create(Platform& platform);

	protected:
		InputManager(Platform& platform);

		TouchInputManager* touch_manager_;
		Keyboard* keyboard_;
		SonyControllerInputManager* controller_manager_;
	};
}

#endif // !_GEF_INPUT_INPUT_MANAGER_H
