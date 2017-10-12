#include "anim_app.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <maths/vector2.h>
#include <input/input_manager.h>
#include <input/sony_controller_input_manager.h>
#include <input/keyboard.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include <graphics/scene.h>
#include <animation/skeleton.h>
#include <animation/animation.h>

AnimApp::AnimApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(NULL),
	font_(NULL),
	renderer_3d_(NULL),
	model_scene_(NULL),
	skeleton_(NULL),
	running_anim_(NULL),
	idle_anim_(NULL)
{
}

void AnimApp::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	input_manager_ = gef::InputManager::Create(platform_);

	InitFont();

	// create a new scene object and read in the data from the file
	// no meshes or materials are created yet
	// we're not making any assumptions about what the data may be loaded in for
	model_scene_ = new gef::Scene();
	model_scene_->ReadSceneFromFile(platform_, "Y_Bot.scn");

	// we do want to render the data stored in the scene file so lets create the materials from the material data present in the scene file
	model_scene_->CreateMaterials(platform_);

	// now check to see if there is any mesh data in the file, if so lets create a mesh from it
	if (model_scene_->meshes.size() > 0)
	{
		mesh_ = model_scene_->CreateMesh(platform_, model_scene_->meshes.front());

		// get the player mesh instance to use this mesh for drawing
		player_.set_mesh(mesh_);
	}

	// check to see if there is a skeleton in the the scene file
	// if so, pull out the bind pose and create an array of matrices
	// that wil be used to store the bone transformations
	if (model_scene_->skeletons.size() > 0)
	{
		skeleton_ = model_scene_->skeletons.front();
		bind_pose_.CreateBindPose(skeleton_);
		bone_matrices_.resize(skeleton_->joints().size());
	}

	// anims

	// initialise the animation player with the bind pose
	anim_player_.Update(0.0f, bind_pose_);

	running_anim_ = LoadAnimation("running_InPlace.scn");
	idle_anim_ = LoadAnimation("idle.scn");

	// start with the character facing the right
	face_right_ = true;

	SetupCamera();
	SetupLights();
}

void AnimApp::CleanUp()
{
	bone_matrices_.clear();

	delete running_anim_;
	running_anim_ = NULL;

	delete idle_anim_;
	idle_anim_ = NULL;

	delete model_scene_;
	model_scene_ = NULL;

	delete mesh_;
	mesh_ = NULL;

	CleanUpFont();

	delete input_manager_;
	input_manager_ = NULL;

	delete sprite_renderer_;
	sprite_renderer_ = NULL;

	delete renderer_3d_;
	renderer_3d_ = NULL;
}

bool AnimApp::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;


	float forward = 0.0f;


	// read input devices to control the character
	if (input_manager_)
	{
		input_manager_->Update();

		// if there is a controller connected then update the direction of the character from
		// the left stick
		gef::SonyControllerInputManager* controller_manager = input_manager_->controller_input();
		if (controller_manager)
		{
			const gef::SonyController* controller = controller_manager->GetController(0);
			if (controller)
				forward = controller->left_stick_x_axis();
		}

		// if there is a keyboard, check the arrow keys to control the direction of the character
		gef::Keyboard* keyboard = input_manager_->keyboard();
		if (keyboard)
		{
			if (keyboard->IsKeyDown(gef::Keyboard::KC_RIGHT))
				forward = 1.0f;
			else if (keyboard->IsKeyDown(gef::Keyboard::KC_LEFT))
				forward = -1.0f;
		}
	}

	// store the animation used on the last frame so we can detect if
	// we need to trigger a animation change in this frame
	const gef::Animation* previous_anim = anim_player_.clip();

	// forward == 0.0f, character is in idle state
	// otherwise the character is moving left or right
	if (forward == 0.0f)
	{
		// check to see if we are transiting to idle from a different animation
		// if so start playing idle from the beginning
		if (previous_anim != idle_anim_)
		{
			anim_player_.set_clip(idle_anim_);
			anim_player_.set_looping(true);
			anim_player_.set_anim_time(0.0f);
		}
	}
	else
	{
		if (forward > 0.0f)
			face_right_ = true;
		else
			face_right_ = false;

		// if previous animation is different from running animation the
		// start playing the run animation from the start
		if (previous_anim != running_anim_)
		{
			anim_player_.set_clip(running_anim_);
			anim_player_.set_looping(true);
			anim_player_.set_anim_time(0.0f);
		}
	}

	if (skeleton_)
	{
		anim_player_.Update(frame_time, bind_pose_);

		// calculate bone matrices that need to be passed to the shader
		// this should be the final pose if multiple animations are blended together
		std::vector<gef::Matrix44>::const_iterator pose_matrix_iter = anim_player_.pose().global_pose().begin();
		std::vector<gef::Joint>::const_iterator joint_iter = skeleton_->joints().begin();

		for (std::vector<gef::Matrix44>::iterator bone_matrix_iter = bone_matrices_.begin(); bone_matrix_iter != bone_matrices_.end(); ++bone_matrix_iter, ++joint_iter, ++pose_matrix_iter)
			*bone_matrix_iter = (joint_iter->inv_bind_pose * *pose_matrix_iter);
	}

	// set the transformation matrix for the character based on the way they are facing
	gef::Matrix44 player_transform;
	if (face_right_)
		player_transform.SetIdentity();
	else
		player_transform.RotationY(gef::DegToRad(180.0f));
	player_.set_transform(player_transform);

	return true;
}

void AnimApp::Render()
{
	gef::Matrix44 projection_matrix;
	gef::Matrix44 view_matrix;

	projection_matrix = platform_.PerspectiveProjectionFov(camera_fov, (float)platform_.width() / (float)platform_.height(), near_plane, far_plane);
	view_matrix.LookAt(camera_eye, camera_lookat, camera_up);
	renderer_3d_->set_projection_matrix(projection_matrix);
	renderer_3d_->set_view_matrix(view_matrix);

	// draw meshes here
	renderer_3d_->Begin();

	// draw the player, the pose is defined by the bone matrices
	renderer_3d_->DrawSkinnedMesh(player_, bone_matrices_);
	renderer_3d_->End();

	// setup the sprite renderer, but don't clear the frame buffer
	// draw 2D sprites here
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}
void AnimApp::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void AnimApp::CleanUpFont()
{
	delete font_;
	font_ = NULL;
}

void AnimApp::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void AnimApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void AnimApp::SetupCamera()
{
	// initialise the camera settings
	camera_eye = gef::Vector4(-5.0f, 1.0f, 0.0f);
	camera_lookat = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_up = gef::Vector4(0.0f, 1.0f, 0.0f);
	camera_fov = gef::DegToRad(45.0f);
	near_plane = 0.01f;
	far_plane = 1000.f;
}

gef::Animation* AnimApp::LoadAnimation(const char* anim_scene_filename, const char* anim_name)
{
	gef::Animation* anim = NULL;

	gef::Scene anim_scene;
	if (anim_scene.ReadSceneFromFile(platform_, anim_scene_filename))
	{
		// if the animation name is specified then try and find the named anim
		// otherwise return the first animation if there is one
		std::map<gef::StringId, gef::Animation*>::const_iterator anim_node_iter;
		if (anim_name)
			anim_node_iter = anim_scene.animations.find(gef::GetStringId(anim_name));
		else
			anim_node_iter = anim_scene.animations.begin();

		if (anim_node_iter != anim_scene.animations.end())
			anim = new gef::Animation(*anim_node_iter->second);
	}

	return anim;
}

