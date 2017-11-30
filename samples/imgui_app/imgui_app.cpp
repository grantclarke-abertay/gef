#include "imgui_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>

#include <imgui.h>

void ImGui_NewFrame();

ImguiApp::ImguiApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL)
{
}

void ImguiApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);

	InitFont();
}

void ImguiApp::CleanUp()
{
	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;
}

bool ImguiApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

    ImGui_NewFrame();

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) show_test_window ^= 1;
        if (ImGui::Button("Another Window")) show_another_window ^= 1;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


        platform_.set_render_target_clear_colour(gef::Colour(clear_color.x, clear_color.y, clear_color.z, clear_color.w));
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }



	return true;
}

void ImguiApp::Render()
{
	sprite_renderer_->Begin();
	DrawHUD();
	sprite_renderer_->End();

    ImGui::Render();
}
void ImguiApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void ImguiApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void ImguiApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

