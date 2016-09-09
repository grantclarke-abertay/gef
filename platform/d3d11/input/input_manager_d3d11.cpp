#include "input_manager_d3d11.h"
#include "touch_input_manager_d3d11.h"
#include "keyboard_d3d11.h"
#include "sony_controller_input_manager_d3d11.h"
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	InputManagerD3D11::InputManagerD3D11(Platform& platform)
		: InputManager(platform)
		, platform_(platform)
		, direct_input_(NULL)
	{
		HRESULT hresult = S_OK;
		// Setup DirectInput
		hresult = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&direct_input_, NULL);

		if (hresult == S_OK)
		{
			touch_manager_ = new TouchInputManagerD3D11(&reinterpret_cast<const PlatformD3D11&>(platform), direct_input_);
			platform.set_touch_input_manager(touch_manager_);
			keyboard_ = new KeyboardD3D11(direct_input_);
			controller_manager_ = new SonyControllerInputManagerD3D11(reinterpret_cast<const PlatformD3D11&>(platform), direct_input_);
		}
	}

	InputManagerD3D11::~InputManagerD3D11()
	{
		delete touch_manager_;
		platform_.set_touch_input_manager(NULL);

		delete keyboard_;
		delete controller_manager_;

		if (direct_input_)
			direct_input_->Release();
	}

	void InputManagerD3D11::Update()
	{
		InputManager::Update();
	}
}