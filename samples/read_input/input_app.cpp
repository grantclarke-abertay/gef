#include "input_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/keyboard.h>
#include <input/sony_controller_input_manager.h>
#include <system/debug_log.h>

static const char* key_names[] =
{
"0",
"1",
"2",
"3",
"4",
"5",
"6",
"7",
"8",
"9",
"A",
"B",
"C",
"D",
"E",
"F",
"G",
"H",
"I",
"J",
"K",
"L",
"M",
"N",
"O",
"P",
"Q",
"R",
"S",
"T",
"U",
"V",
"W",
"X",
"Y",
"Z",
"F1",
"F2",
"F3",
"F4",
"F5",
"F6",
"F7",
"F8",
"F9",
"F10",
"F11",
"F12",
"F13",
"F14",
"F15",
"NUMPAD0",
"NUMPAD1",
"NUMPAD2",
"NUMPAD3",
"NUMPAD4",
"NUMPAD5",
"NUMPAD6",
"NUMPAD7",
"NUMPAD8",
"NUMPAD9",
"NUMPADENTER",
"NUMPADSTAR",
"NUMPADEQUALS",
"NUMPADMINUS",
"NUMPADPLUS",
"NUMPADPERIOD",
"NUMPADSLASH",
"ESCAPE",
"TAB",
"LSHIFT",
"RSHIFT",
"LCONTROL",
"RCONTROL",
"BACKSPACE",
"RETURN",
"LALT",
"SPACE",
"CAPSLOCK",
"NUMLOCK",
"SCROLL",
"RALT",
"AT",
"COLON",
"UNDERLINE",
"MINUS",
"EQUALS",
"LBRACKET",
"RBRACKET",
"SEMICOLON",
"APOSTROPHE",
"GRAVE",
"BACKSLASH",
"COMMA",
"PERIOD",
"SLASH"
};


InputApp::InputApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	input_manager_(NULL)
{
}

void InputApp::Init()
{
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	input_manager_ = platform_.CreateInputManager();


	InitFont();
}

void InputApp::CleanUp()
{
	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool InputApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	if (input_manager_)
	{
		input_manager_->Update();

		// keyboard
		{
			const gef::Keyboard* keyboard = input_manager_->keyboard();
			if (keyboard)
			{
				for (int key_num = 0; key_num < gef::Keyboard::NUM_KEY_CODES; ++key_num)
				{
					if (keyboard->IsKeyPressed((gef::Keyboard::KeyCode)key_num))
						gef::DebugOut("%s pressed\n", key_names[key_num]);
					else if (keyboard->IsKeyReleased((gef::Keyboard::KeyCode)key_num))
						gef::DebugOut("%s released\n", key_names[key_num]);
				}
			}
		}

		// controller
		{
			const gef::SonyControllerInputManager* controller_input = input_manager_->controller_input();
			if (controller_input)
			{
				const gef::SonyController* controller = controller_input->GetController(0);
				if (controller)
				{
					if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS)
						gef::DebugOut("CROSS pressed\n");
					if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
						gef::DebugOut("SQUARE pressed\n");
					if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
						gef::DebugOut("TRIANGLE pressed\n");
					if (controller->buttons_pressed() & gef_SONY_CTRL_CIRCLE)
						gef::DebugOut("CIRCLE pressed\n");

					if (controller->left_stick_x_axis() != 0.0f)
						gef::DebugOut("left_stick_x_axis: %f\n", controller->left_stick_x_axis());
					if (controller->left_stick_y_axis() != 0.0f)
						gef::DebugOut("left_stick_y_axis: %f\n", controller->left_stick_y_axis());

					if (controller->right_stick_x_axis() != 0.0f)
						gef::DebugOut("right_stick_x_axis: %f\n", controller->right_stick_x_axis());
					if (controller->right_stick_y_axis() != 0.0f)
						gef::DebugOut("right_stick_y_axis: %f\n", controller->right_stick_y_axis());
				}
			}
		}

	}

	return true;
}

void InputApp::Render()
{
	sprite_renderer_->Begin();
	DrawHUD();
	sprite_renderer_->End();
}
void InputApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void InputApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void InputApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector3(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

