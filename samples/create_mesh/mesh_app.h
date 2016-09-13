#ifndef _MESH_APP_H
#define _MESH_APP_H

#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
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

	gef::SpriteRenderer* sprite_renderer_;
	gef::Font* font_;

	float fps_;

	class gef::Renderer3D* renderer_3d_;
	class gef::Mesh* mesh_;
	gef::MeshInstance cube_player_;

	gef::Vector4 camera_eye;
	gef::Vector4 camera_lookat;
	gef::Vector4 camera_up;
	float camera_fov;
	float near_plane;
	float far_plane;
};

#endif // _MESH_APP_H
