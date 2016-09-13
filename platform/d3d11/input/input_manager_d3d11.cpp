#include "input_manager_d3d11.h"
#include "touch_input_manager_d3d11.h"
#include "keyboard_d3d11.h"
#include "sony_controller_input_manager_d3d11.h"
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	InputManager* InputManager::Create(Platform& platform)
	{
		return new InputManagerD3D11(platform);
	}

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
			PlatformD3D11& platform_d3d11 = reinterpret_cast<PlatformD3D11&>(platform);

			touch_manager_ = new TouchInputManagerD3D11(&platform_d3d11, direct_input_);
			platform.set_touch_input_manager(touch_manager_);
			keyboard_ = new KeyboardD3D11(platform_d3d11, direct_input_);
			controller_manager_ = new SonyControllerInputManagerD3D11(platform_d3d11, direct_input_);
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

}