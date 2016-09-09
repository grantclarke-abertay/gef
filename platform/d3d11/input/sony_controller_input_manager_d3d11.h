#ifndef _GEF_SONY_CONTROLLER_INPUT_MANAGER_D3D11_H
#define _GEF_SONY_CONTROLLER_INPUT_MANAGER_D3D11_H

#include <input/sony_controller_input_manager.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <dinputd.h>

namespace gef
{
	class PlatformD3D11;

	class SonyControllerInputManagerD3D11 : public SonyControllerInputManager
	{
	public:
		SonyControllerInputManagerD3D11(const PlatformD3D11& platform, LPDIRECTINPUT8 direct_input);
		~SonyControllerInputManagerD3D11();

		static BOOL CALLBACK enumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance, VOID* pContext );
		static BOOL CALLBACK enumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext );


		Int32 Update();

	private:
		void UpdateController(SonyController& controller, const DIJOYSTATE2& joystate);
		void CleanUp();

		LPDIRECTINPUT8			direct_input_;
		LPDIRECTINPUTDEVICE8	joystick_;

		static float kStickDeadZone;
	};
}

#endif // _GEF_SONY_CONTROLLER_INPUT_MANAGER_D3D11_H
