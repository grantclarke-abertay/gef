#ifndef _MESH_APP_H
#define _MESH_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include "depth_shader.h"
#include "shadow_shader.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
	class RenderTarget;
	class DepthBuffer;
}

class MeshApp : public gef::Application
{
public:
	MeshApp(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
private:
	void InitFont();
	void CleanUpFont();
	void DrawHUD();
	void SetupLights();
	void SetupCamera();

	gef::Mesh* CreateCubeMesh();

	void InitShadow();
	void ReleaseShadow();
	void RenderShadow(const gef::Vector4& look_at, const float distance_to_light);
	void SetupShader(const gef::MeshInstance& mesh_instance);

	void Render3DScene();


	class gef::Renderer3D* renderer_3d_;
	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;

	float fps_;

	// objects
	class gef::Mesh* mesh_;
	gef::MeshInstance floor_;
	gef::MeshInstance box_;

	// camera setup
	gef::Vector4 camera_eye_;
	gef::Vector4 camera_lookat_;
	gef::Vector4 camera_up_;
	float camera_fov_;
	float near_plane_;
	float far_plane_;

	// shadow rendering
	gef::RenderTarget* shadow_render_target_;
	gef::DepthBuffer* shadow_depth_buffer_;
	gef::Sprite shadow_sprite_;
	DepthShader* depth_shader_;
	ShadowShader* shadow_shader_;
	gef::Matrix44 light_projection_matrix_;
	gef::Matrix44 light_view_matrix_;
};

#endif // _MESH_APP_H
