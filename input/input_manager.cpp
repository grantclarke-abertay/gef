#include "input_manager.h"
#include "touch_input_manager.h"
#include "keyboard.h"
#include "sony_controller_input_manager.h"
#include <cstddef> // for NULL definition

namespace gef
{
	InputManager::InputManager(Platform& platform)
		: touch_manager_(NULL)
		, keyboard_(NULL)
		, controller_manager_(NULL)
	{

	}

	InputManager::~InputManager()
	{
		// derived classes will be responsible for freeing up input managers
		// in case they have to free up resources after other input managers have been freed
	}

	void InputManager::Update()
	{
		if (touch_manager_)
			touch_manager_->Update();

		if (keyboard_)
			keyboard_->Update();

		if (controller_manager_)
			controller_manager_->Update();
	}
}