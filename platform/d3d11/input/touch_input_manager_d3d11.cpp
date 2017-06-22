#include <platform/d3d11/input/touch_input_manager_d3d11.h>
#include <gef.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/win32/system/window_win32.h>

namespace gef
{
#define INVALID_TOUCH_ID		(-1)


	TouchInputManagerD3D11::TouchInputManagerD3D11(const PlatformD3D11* platform, LPDIRECTINPUT8 direct_input) :
		direct_input_(direct_input),
		mouse_(NULL),
		platform_(platform)
	{
		HRESULT hresult = S_OK;
		hresult = direct_input_->CreateDevice(GUID_SysMouse, &mouse_, NULL);

		if(SUCCEEDED(hresult))
			hresult = mouse_->SetDataFormat(&c_dfDIMouse);

		if(SUCCEEDED(hresult))
			hresult = mouse_->SetCooperativeLevel(platform_->top_level_hwnd(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);


		if(SUCCEEDED(hresult))
		{
			DIPROPDWORD dipdw;
			dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
			dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			dipdw.diph.dwObj        = 0;
			dipdw.diph.dwHow        = DIPH_DEVICE;
			dipdw.dwData = 16;

			hresult = mouse_->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

			dipdw.dwData = DIPROPAXISMODE_REL;
			hresult = mouse_->SetProperty(DIPROP_AXISMODE, &dipdw.diph);

		}

		if(FAILED(hresult))
			CleanUp();
/*
		for(Int32 panel_index=0;panel_index<1;++panel_index)
		{
			panel_enabled_[panel_index] = false;

			for(Int32 touch_index=0;touch_index<2;++touch_index)
			{
				Touch* touch = &touches_[panel_index][touch_index];
				touch->type = TT_NONE;
				touch->id = INVALID_TOUCH_ID;
				is_button_down_[touch_index] = false;
			}
		}
*/
		panels_.push_back(TouchContainer());

		is_button_down_[0] = false;
		is_button_down_[1] = false;
		is_button_down_[2] = false;
		is_button_down_[3] = false;
	}

	TouchInputManagerD3D11::~TouchInputManagerD3D11()
	{
		CleanUp();
	}

	void TouchInputManagerD3D11::CleanUp()
	{
		if (mouse_)
		{
			mouse_->Unacquire();
			mouse_->Release();
			mouse_ = NULL;
		}
	}

	void TouchInputManagerD3D11::Update()
	{
		HRESULT hresult = S_OK;
		mouse_->Acquire();

		// store the button status for this update
		// intialise it to the same state as last frame
		bool is_button_down[4] = { false, false, false, false };

		is_button_down[0] = is_button_down_[0];
		is_button_down[1] = is_button_down_[1];
		is_button_down[2] = is_button_down_[2];
		is_button_down[3] = is_button_down_[3];

		DIMOUSESTATE mouse_state;
		if (mouse_->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state) == S_OK)
		{
			mouse_rel_.set_x(mouse_state.lX);
			mouse_rel_.set_y(mouse_state.lY);
			mouse_rel_.set_z(mouse_state.lZ);

			is_button_down[0] = mouse_state.rgbButtons[0] & 0x80 ? true : false;
			is_button_down[1] = mouse_state.rgbButtons[1] & 0x80 ? true : false;
			is_button_down[2] = mouse_state.rgbButtons[2] & 0x80 ? true : false;
			is_button_down[3] = mouse_state.rgbButtons[3] & 0x80 ? true : false;
		}

/*
		DIDEVICEOBJECTDATA od;
		DWORD dwElements = 1;



		while(dwElements != 0)
		{
			dwElements = 1;
			hresult = mouse_->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od, &dwElements, 0);
			if(FAILED(hresult))
				break;

			switch(od.dwOfs)
			{
			case DIMOFS_BUTTON0:
				if(od.dwData & 0x80)
					is_button_down[0] = true;
				else
					is_button_down[0] = false;
				break;
			case DIMOFS_BUTTON1:
				if(od.dwData & 0x80)
					is_button_down[1] = true;
				else
					is_button_down[1] = false;
				break;
			}
		}
*/

		POINT mouse_pos;
		GetCursorPos(&mouse_pos);
		ScreenToClient(platform_->hwnd(), &mouse_pos);
		mouse_position_.x = (float)mouse_pos.x;
		mouse_position_.y = (float)mouse_pos.y;

		for(Int32 panel_index=0;panel_index<1;++panel_index)
		{
			if(panel_enabled_[panel_index])
			{
				// go through existing touches and remove any that were released last update
				for (TouchIterator touch = panels_[panel_index].begin(); touch != panels_[panel_index].end();)
				{
					if (touch->type == TT_RELEASED)
						touch = panels_[panel_index].erase(touch);
					else
						++touch;
				}

				// get the touch data for this panel
				// check to see if we have any data to update existing touches
				for(Int32 touch_index=0;touch_index<2;++touch_index)
				{
					Touch touch;
					touch.id = touch_index;
					touch.position = mouse_position_;


					if(is_button_down[touch_index])
					{
						if(!is_button_down_[touch_index])
						{
							// NEEDS TESTING
							// make sure mouse_position is actually within drawable area 
							if( (mouse_pos.y >= 0) && (mouse_pos.y < platform_->height()) && (mouse_pos.x >= 0) && (mouse_pos.x < platform_->width()))
							{
								// new touch
								touch.type = TT_NEW;
								AddTouch(panel_index, touch);
							}
						}
						else
						{
							// touch updated
							touch.type = TT_ACTIVE;
							UpdateTouch(panel_index, touch);
						}
					}
					else
					{
						if(is_button_down_[touch_index])
						{
							// touch released
							touch.type = TT_RELEASED;
							UpdateTouch(panel_index, touch);
						}
					}

				}
			}
		}

		is_button_down_[0] = is_button_down[0];
		is_button_down_[1] = is_button_down[1];
		is_button_down_[2] = is_button_down[2];
		is_button_down_[3] = is_button_down[3];

	}

	void TouchInputManagerD3D11::EnablePanel(const Int32 panel_index)
	{
		panel_enabled_[panel_index] = true;
	}

	void TouchInputManagerD3D11::DisablePanel(const Int32 panel_index)
	{
		panel_enabled_[panel_index] = false;
	}
}

