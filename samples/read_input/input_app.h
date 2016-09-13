#ifndef _INPUT_APP_H
#define _INPUT_APP_H

#include <system/application.h>
#include <maths/vector2.h>
#include <input/touch_input_manager.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class InputManager;
}

class InputApp : public gef::Application
{
public:
	InputApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void ProcessKeyboardInput();
	void ProcessControllerInput();
	void ProcessTouchInput();

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;
	gef::InputManager* input_manager_;

	Int32 active_touch_id_;
	gef::Vector2 touch_position_;

	float fps_;
};

#endif // _INPUT_APP_H
