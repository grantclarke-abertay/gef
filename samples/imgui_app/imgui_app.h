#ifndef _MESH_APP_H
#define _MESH_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <imgui.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
}

class ImguiApp : public gef::Application
{
public:
	ImguiApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();

	gef::Mesh* CreateCubeMesh();

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;

	float fps_;

	bool show_test_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImColor(114, 144, 154);
};

#endif // _MESH_APP_H
