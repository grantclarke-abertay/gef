#ifndef _PLATFORM_D3D11_INPUT_INPUT_MANAGER_H
#define _PLATFORM_D3D11_INPUT_INPUT_MANAGER_H

#include <input/input_manager.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dinputd.h>

namespace gef
{
	class InputManagerD3D11 : public InputManager
	{
	public:
		InputManagerD3D11(Platform& platform);
		~InputManagerD3D11();
	protected:
		Platform& platform_;
		LPDIRECTINPUT8 direct_input_;
	};
}
#endif // !_PLATFORM_D3D11_INPUT_INPUT_MANAGER_H
