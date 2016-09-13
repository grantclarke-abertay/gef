#include "mesh_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/touch_input_manager.h>
#include <maths/vector2.h>
#include <input/sony_controller_input_manager.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>

MeshApp::MeshApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL)
{
}

void MeshApp::Init()
{
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	renderer_3d_ = platform_.CreateRenderer3D();

	InitFont();

	mesh_ = CreateCubeMesh();
	cube_player_.set_mesh(mesh_);

	SetupCamera();
	SetupLights();
}

void MeshApp::CleanUp()
{
	delete mesh_;
	mesh_ = NULL;

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;
}

bool MeshApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

	return true;
}

void MeshApp::Render()
{
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;

	projection_matrix = platform_.PerspectiveProjectionFov(camera_fov, (float)platform_.width() / (float)platform_.height(), near_plane, far_plane);
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_projection_matrix(projection_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw meshes here
	renderer_3d_->Begin();
	renderer_3d_->DrawMesh(cube_player_);
	renderer_3d_->End();

	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}
void MeshApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void MeshApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void MeshApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

gef::Mesh* MeshApp::CreateCubeMesh()
{
	gef::Mesh* mesh = platform_.CreateMesh();

	// initialise the vertex data to create a 1, 1, 1 cube
	const float half_size = 0.5f;
	const gef::Mesh::Vertex vertices[] = {
		{ half_size, -half_size, -half_size,  0.577f, -0.577f, -0.577f, 0.0f, 0.0f},
		{ half_size,  half_size, -half_size,  0.577f,  0.577f, -0.577f, 0.0f, 0.0f},
		{-half_size,  half_size, -half_size, -0.577f,  0.577f, -0.577f, 0.0f, 0.0f},
		{-half_size, -half_size, -half_size, -0.577f, -0.577f, -0.577f, 0.0f, 0.0f},
		{ half_size, -half_size,  half_size,  0.577f, -0.577f,  0.577f, 0.0f, 0.0f},
		{ half_size,  half_size,  half_size,  0.577f,  0.577f,  0.577f, 0.0f, 0.0f},
		{-half_size,  half_size,  half_size, -0.577f,  0.577f,  0.577f, 0.0f, 0.0f},
		{-half_size, -half_size,  half_size, -0.577f, -0.577f,  0.577f, 0.0f, 0.0f}	
	};

	mesh->InitVertexBuffer(platform_, static_cast<const void*>(vertices), sizeof(vertices)/sizeof(gef::Mesh::Vertex), sizeof(gef::Mesh::Vertex));

	// we will create a single triangle list primitive to draw the triangles that make up the cube
	mesh->AllocatePrimitives(1);
	gef::Primitive* primitive = mesh->GetPrimitive(0);

	const UInt32 indices[] = {
    // Back
    0, 1, 2,
    2, 3, 0,
    // Front
    6, 5, 4,
    4, 7, 6,
    // Left
    2, 7, 3,
    2, 6, 7,
    // Right
    0, 4, 1,
	5, 1, 4,
    // Top
    6, 2, 1, 
	5, 6, 1,
	// Bottom
    0, 3, 7,
    0, 7, 4 
	};

	primitive->InitIndexBuffer(platform_, static_cast<const void*>(indices), sizeof(indices)/sizeof(UInt32),sizeof(UInt32));
	primitive->set_type(gef::TRIANGLE_LIST);

	// set size of bounds, we need this for collision detection routines
	gef::Aabb aabb(gef::Vector4(-half_size, -half_size, -half_size), gef::Vector4(half_size, half_size, half_size));
	gef::Sphere sphere(aabb);

	mesh->set_aabb(aabb);
	mesh->set_bounding_sphere(sphere);

	return mesh;
}

void MeshApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void MeshApp::SetupCamera()
{
	// initialise the camera settings
	camera_eye = gef::Vector4(5.0f, 5.0f, 5.0f);
	camera_lookat = gef::Vector4(0.0f, 0.0f, 0.0f);
	camera_up = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov = gef::DegToRad(45.0f);
	near_plane = 0.01f;
	far_plane = 1000.f;
}
