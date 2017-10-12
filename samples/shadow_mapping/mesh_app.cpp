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
#include <graphics/render_target.h>
#include <graphics/depth_buffer.h>

MeshApp::MeshApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	shadow_render_target_(NULL),
	shadow_depth_buffer_(NULL),
	depth_shader_(NULL),
	shadow_shader_(NULL)
{
}

void MeshApp::Init()
{
	sprite_renderer_ = platform_.CreateSpriteRenderer();
	renderer_3d_ = platform_.CreateRenderer3D();

	InitFont();

	mesh_ = CreateCubeMesh();
	
	// create the floor object
	{
		gef::Matrix44 transform;
		transform.Scale(gef::Vector4(10.0f, 1.0f, 10.0f));
		transform.SetTranslation(gef::Vector4(0.0f, -0.5f, 0.0f));
		floor_.set_transform(transform);
		floor_.set_mesh(mesh_);
	}

	// create the box object that will cast a shadow on the floor
	{
		gef::Matrix44 transform;
		transform.Scale(gef::Vector4(1.0f, 1.0f, 1.0f));
		transform.SetTranslation(gef::Vector4(-2.5f, 0.5f, 2.5f));
		box_.set_transform(transform);
		box_.set_mesh(mesh_);
	}

	SetupCamera();
	SetupLights();

	InitShadow();
}

void MeshApp::CleanUp()
{
	ReleaseShadow();

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
	// render the scene to the shadow buffer
	const float kDistanceToLight = 15.0f;
	RenderShadow(gef::Vector4(0.0f, 0.0f, 0.0f), kDistanceToLight);

	// calculate view matrix given camera parameters
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);

	// calculate projection matrix from camera parameters
	gef::Matrix44 projection_matrix;
	projection_matrix = platform_.PerspectiveProjectionFov(camera_fov_, (float)platform_.width() / (float)platform_.height(), near_plane_, far_plane_);

	// tell renderer to use camera projection and view matrices
	renderer_3d_->set_projection_matrix(projection_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// store the current shader in case we need to restore it
	gef::Shader* prev_shader = renderer_3d_->shader();

	// if the shadow shader is initialised the we'll use it
	if (shadow_shader_ && shadow_render_target_)
	{
		shadow_shader_->primitive_data().depth_texture = shadow_render_target_->texture();
		renderer_3d_->SetShader(shadow_shader_);
	}

	// draw meshes here
	renderer_3d_->Begin();

	Render3DScene();

	renderer_3d_->End();

	// restore previous shader
	renderer_3d_->SetShader(prev_shader);

	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here
	sprite_renderer_->SetShader(NULL);
	sprite_renderer_->Begin(false);

	// add this line back in if you want to see what is being
	// renderered to the shadow buffer
	sprite_renderer_->DrawSprite(shadow_sprite_);

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
	camera_eye_ = gef::Vector4(8.0f, 6.0f, 11.0f);
	camera_lookat_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	camera_up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov_ = gef::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 1000.f;
}

void MeshApp::InitShadow()
{
	// shadow buffer needs to be square
	// because it's a different size from the frame buffer
	// we need to create a depth buffer to go with it too
	const int kShadowBufferSize = 1024;
	shadow_render_target_ = gef::RenderTarget::Create(platform_, kShadowBufferSize, kShadowBufferSize);
	shadow_depth_buffer_ = gef::DepthBuffer::Create(platform_, kShadowBufferSize, kShadowBufferSize);

	// create shader to use when render depth values to shadow buffer
	depth_shader_ = new DepthShader(platform_);
	platform_.AddShader(depth_shader_);

	// create shader that will render the scene with the shadow buffer
	shadow_shader_ = new ShadowShader(platform_);
	platform_.AddShader(shadow_shader_);

	// for debug purposes
	shadow_sprite_.set_texture(shadow_render_target_->texture());
	shadow_sprite_.set_width(512.0f);
	shadow_sprite_.set_height(512.0f);
	shadow_sprite_.set_position(shadow_sprite_.width()*0.5f, shadow_sprite_.height()*0.5f, 0.0f);
}

void MeshApp::ReleaseShadow()
{
	delete shadow_depth_buffer_;
	shadow_depth_buffer_ = NULL;

	delete shadow_render_target_;
	shadow_render_target_ = NULL;

	platform_.RemoveShader(shadow_shader_);
	delete shadow_shader_;
	shadow_shader_ = NULL;

	platform_.RemoveShader(depth_shader_);
	delete depth_shader_;
	depth_shader_ = NULL;

	shadow_sprite_.set_texture(NULL);
}

void MeshApp::RenderShadow(const gef::Vector4& look_at, const float distance_to_light)
{
	if (shadow_render_target_)
	{
		// render to the shadow map render target
		platform_.set_render_target(shadow_render_target_);
		platform_.set_depth_buffer(shadow_depth_buffer_);

		// Setup field of view and screen aspect for a square light source.
		float fov = (float)FRAMEWORK_PI / 2.0f;
		float aspect = 1.0f;

		// create projection matrix for the light
		light_projection_matrix_ = platform_.PerspectiveProjectionFov(fov, aspect, 1.0f, far_plane_);

		// using the light position as the eye position for rendering will often result in geometry being too small
		// in shadow render target
		// use a scaled vector pointing to the light position instead
		gef::Vector4 to_light = renderer_3d_->default_shader_data().GetPointLight(0).position() - look_at;
		to_light.Normalise();

		// calculate the view matrix when rendering from the light
		gef::Vector4 eye = look_at + to_light*distance_to_light;
		light_view_matrix_.LookAt(eye, look_at, gef::Vector4(0.0f, 1.0f, 0.0f));

		// grab the current shader so we can restore it later
		gef::Shader* previous_shader = renderer_3d_->shader();

		// set the renderer to use light matrics
		renderer_3d_->set_projection_matrix(light_projection_matrix_);
		renderer_3d_->set_view_matrix(light_view_matrix_);

		// use the shader for renderering the depth values to the shadow buffer
		renderer_3d_->SetShader(depth_shader_);

		// render target needs to be cleared to zero [black]
		platform_.set_render_target_clear_colour(gef::Colour(0.0f, 0.0f, 0.0f, 1.0f));
		renderer_3d_->Begin();

		Render3DScene();

		renderer_3d_->End();

		// restore previous shader
		renderer_3d_->SetShader(previous_shader);

		// set render target to the the default [the back buffer]
		platform_.set_render_target(NULL);
		platform_.set_depth_buffer(NULL);

		// reset clear colour
		platform_.set_render_target_clear_colour(gef::Colour(0.0f, 0.0f, 1.0f, 1.0f));
	}
}


void MeshApp::SetupShader(const gef::MeshInstance& mesh_instance)
{
	// depending on the shader being used
	// pass the data to the appropriate shader
	if (renderer_3d_->shader() == depth_shader_)
	{
		depth_shader_->SetVertexShaderData(mesh_instance.transform(), light_view_matrix_, light_projection_matrix_);
	}
	else if (renderer_3d_->shader() == shadow_shader_)
	{
		shadow_shader_->SetMeshData(
			renderer_3d_->default_shader_data(),
			mesh_instance.transform(),
			renderer_3d_->inv_world_transpose_matrix(),
			renderer_3d_->view_matrix(),
			renderer_3d_->projection_matrix(),
			light_view_matrix_,
			light_projection_matrix_);
	}
}

void MeshApp::Render3DScene()
{
	// set any shader variables for this object
	SetupShader(box_);
	renderer_3d_->DrawMesh(box_);

	SetupShader(floor_);
	renderer_3d_->DrawMesh(floor_);
}
