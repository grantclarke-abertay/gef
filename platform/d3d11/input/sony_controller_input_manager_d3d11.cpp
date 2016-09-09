#include <platform/d3d11/input/sony_controller_input_manager_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	float SonyControllerInputManagerD3D11::kStickDeadZone = 0.1f;

	BOOL CALLBACK SonyControllerInputManagerD3D11::enumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pvRef)
	{
		SonyControllerInputManagerD3D11 *context = (SonyControllerInputManagerD3D11 *) pvRef;

		if (0 <= context->direct_input_->CreateDevice(pdidInstance->guidInstance, &context->joystick_, NULL)) {
			return DIENUM_STOP;
		}

		return DIENUM_CONTINUE;
	}

	BOOL CALLBACK SonyControllerInputManagerD3D11::enumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pvRef)
	{
		SonyControllerInputManagerD3D11 *context = (SonyControllerInputManagerD3D11 *) pvRef;

		if (pdidoi->dwType & DIDFT_AXIS) {
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof( DIPROPRANGE );
			diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
			diprg.diph.dwObj = pdidoi->dwType;
			diprg.diph.dwHow = DIPH_BYID;
			diprg.lMin = 0;
			diprg.lMax = 0xff;

			if (0 > context->joystick_->SetProperty(DIPROP_RANGE, &diprg.diph)) {
				return DIENUM_STOP;
			}
		}

		return DIENUM_CONTINUE;
	}

	SonyControllerInputManagerD3D11::SonyControllerInputManagerD3D11(const PlatformD3D11& platform, LPDIRECTINPUT8 direct_input) :
		SonyControllerInputManager(platform),
		direct_input_(direct_input),
		joystick_(NULL)
	{
		HRESULT hresult = S_OK;

		// Find a Joystick Device
		IDirectInputJoyConfig8* joystick_config = NULL;
		hresult = direct_input_->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&joystick_config);

		if (joystick_config)
		{
			joystick_config->Release();
			joystick_config = NULL;
		}

		hresult =  direct_input_->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
		if(SUCCEEDED(hresult))
		{
	
			if (joystick_)
			{
				joystick_->SetDataFormat( &c_dfDIJoystick2 );
				hresult = joystick_->EnumObjects( enumObjectsCallback, this, DIDFT_ALL );
			}
		}

		if(FAILED(hresult))
			CleanUp();
	}

	SonyControllerInputManagerD3D11::~SonyControllerInputManagerD3D11()
	{
		CleanUp();

	}

	void SonyControllerInputManagerD3D11::CleanUp()
	{
		ReleaseNull(joystick_);
	}


	Int32 SonyControllerInputManagerD3D11::Update()
	{
		if (joystick_==NULL) {
			return -1;
		}
		joystick_->Acquire();
		int ret = joystick_->Poll();
		if (ret < 0) {
			return ret;
		}

		DIJOYSTATE2 joystate;
		ret = joystick_->GetDeviceState(sizeof(DIJOYSTATE2), &joystate);
		if (ret < 0) {
			return ret;
		}

		UpdateController(controller_, joystate);

		return 0;
	}

	void SonyControllerInputManagerD3D11::UpdateController(SonyController& controller, const DIJOYSTATE2& joystate)
	{
		UInt32 previous_buttons_down;

		// get the buttons status before they are updated
		previous_buttons_down = controller.buttons_down();
		UInt32 buttons_down = 0;


		float left_stick_x_axis = (float)joystate.lX;
		float left_stick_y_axis = (float)joystate.lY;
		float right_stick_x_axis = (float)joystate.lZ;
		float right_stick_y_axis = (float)joystate.lRz;

		if (0x00 != ( 0x80 & joystate.rgbButtons[9])) {
			buttons_down |= gef_SONY_CTRL_SELECT;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[8])) {
			buttons_down |= gef_SONY_CTRL_START;
		}
		if (31500 == joystate.rgdwPOV[0] ||     0 == joystate.rgdwPOV[0] ||  4500 == joystate.rgdwPOV[0]) {
			buttons_down |= gef_SONY_CTRL_UP;
		}
		if ( 4500 == joystate.rgdwPOV[0] ||  9000 == joystate.rgdwPOV[0] || 13500 == joystate.rgdwPOV[0]) {
			buttons_down |= gef_SONY_CTRL_RIGHT;
		}
		if (13500 == joystate.rgdwPOV[0] || 18000 == joystate.rgdwPOV[0] || 22500 == joystate.rgdwPOV[0]) {
			buttons_down |= gef_SONY_CTRL_DOWN;
		}
		if (22500 == joystate.rgdwPOV[0] || 27000 == joystate.rgdwPOV[0] || 31500 == joystate.rgdwPOV[0]) {
			buttons_down |= gef_SONY_CTRL_LEFT;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[4])) {
			buttons_down |= gef_SONY_CTRL_L1;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[5])) {
			buttons_down |= gef_SONY_CTRL_R1;
		}

		if (0x00 != ( 0x80 & joystate.rgbButtons[6])) {
			buttons_down |= gef_SONY_CTRL_L2;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[7])) {
			buttons_down |= gef_SONY_CTRL_R2;
		}

		if (0x00 != ( 0x80 & joystate.rgbButtons[10])) {
			buttons_down |= gef_SONY_CTRL_L3;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[11])) {
			buttons_down |= gef_SONY_CTRL_R3;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[13])) {
			buttons_down |= gef_SONY_CTRL_TOUCH_PAD;
		}

		if (0x00 != ( 0x80 & joystate.rgbButtons[0] )) {
			buttons_down |= gef_SONY_CTRL_SQUARE;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[1] )) {
			buttons_down |= gef_SONY_CTRL_CROSS;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[2] )) {
			buttons_down |= gef_SONY_CTRL_CIRCLE;
		}
		if (0x00 != ( 0x80 & joystate.rgbButtons[3] )) {
			buttons_down |= gef_SONY_CTRL_TRIANGLE;
		}

		controller.set_buttons_down(buttons_down);
		controller.UpdateButtonStates(previous_buttons_down);

		// calculate the stick values
		// -1 to 1 x-axis left to right
		// -1 to 1 y-axis up to down
		left_stick_x_axis = (static_cast<float>(joystate.lX) - 127.5f) / 127.5f;
		left_stick_y_axis = (static_cast<float>(joystate.lY) - 127.5f) / 127.5f;
		right_stick_x_axis = (static_cast<float>(joystate.lZ) - 127.5f) / 127.5f;
		right_stick_y_axis = (static_cast<float>(joystate.lRz) - 127.5f) / 127.5f;

		// if any of the stick values are less than the dead zone threshold then zero them out
		if(fabsf(left_stick_x_axis) < kStickDeadZone)
			left_stick_x_axis = 0.0f;
		if(fabsf(left_stick_y_axis) < kStickDeadZone)
			left_stick_y_axis = 0.0f;
		if(fabsf(right_stick_x_axis) < kStickDeadZone)
			right_stick_x_axis = 0.0f;
		if(fabsf(right_stick_y_axis) < kStickDeadZone)
			right_stick_y_axis = 0.0f;


		controller.set_left_stick_x_axis(left_stick_x_axis);
		controller.set_left_stick_y_axis(left_stick_y_axis);
		controller.set_right_stick_x_axis(right_stick_x_axis);
		controller.set_right_stick_y_axis(right_stick_y_axis);
	}

}