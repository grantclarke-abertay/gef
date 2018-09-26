#include "assimp_viewer.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/material.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <graphics/vertex_buffer.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <input/keyboard.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include "assimp_scene_loader.h"
#include "primitive_builder.h"



#include <IL/il.h>

void ImGui_NewFrame();

AssimpViewer::AssimpViewer(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(nullptr),
    renderer_3d_(nullptr),
    input_manager_(nullptr),
	font_(nullptr),
    file_dialog_active_(false),
    scene_assets_(nullptr),
	anim_assets_(nullptr),
    file_command_(FC_NONE),
    open_material_num_(-1),
    open_texture_num_(-1),
	file_command_status_(FS_NONE),
	loader_(nullptr),
    textures_window_visible_(true),
	camera_key_(gef::Keyboard::KC_LALT),
	primitive_builder_(nullptr)
{
}

void AssimpViewer::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
    renderer_3d_ = gef::Renderer3D::Create(platform_);

	primitive_builder_ = new PrimitiveBuilder(platform_);

    input_manager_ = gef::InputManager::Create(platform_);

    // make sure if there is a panel to detect touch input, then activate it
    if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
        input_manager_->touch_manager()->EnablePanel(0);


	InitFont();

    SetupCamera();
    SetupLights();

	loader_ = new AssimpSceneLoader();
	loader_->set_flip_winding_order(true);

	// FOR TESTING
	//MainMenuBarFileOpen(
	//	"Hip Hop Dancing.dae",
	//	"");

	//anim_assets_ = new gef::Scene();
	//loader_->ReadAnimation("Hip Hop Dancing.dae", anim_assets_);
	//if (anim_assets_->animations.size() > 0)
	//{
	//	anim_player_.set_clip(anim_assets_->animations.cbegin()->second);
	//	anim_player_.set_looping(true);
	//}

	base_save_directory_ = "";
	load_textures_directory_ = "";
	open_directory_ = "";
	save_directory_ = "";
	open_anim_directory_ = "";
	save_anim_directory_ = "";
}

void AssimpViewer::CleanUp()
{
	delete primitive_builder_;
	primitive_builder_ = nullptr;

	bone_matrices_.clear();

	delete loader_;
	loader_ = nullptr;

    delete scene_assets_;
    scene_assets_ = nullptr;

	delete anim_assets_;
	anim_assets_ = nullptr;

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = nullptr;

    delete renderer_3d_;
    renderer_3d_ = nullptr;
}

bool AssimpViewer::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;



	if (anim_player_.clip())
	{
		anim_player_.Update(frame_time, bind_pose_);
	}

    bool running = true;
    bool open_file_triggered = false;

    ImGui_NewFrame();

    MainMenuBar(running, open_file_triggered);

	LoadOptionsMenu();

	DirectoryPopup();

	
	// if we have an object loaded, enable object UI
    if(scene_assets_ && (scene_assets_->meshes.size() > 0))
    {
		ModelMenu();
//        MaterialsMenu(open_file_triggered);
        // MeshesMenu();

        //if(textures_window_visible_)
        //    TexturesMenu(open_file_triggered);

        //ImGui::Begin("Textures");

        //auto mesh_data = scene_assets_->mesh_data.begin();
        //for (int mesh_num = 0; mesh_num < scene_assets_->mesh_data.size(); ++mesh_num)
        //{
        //    std::string mesh_text = "Mesh: " + std::to_string(mesh_num);
        //    ImGui::Text("%s", mesh_text.c_str());
        //    mesh_data++;
        //}

        //ImGui::End();
    }

    OpenFileDialog(open_file_triggered);
	
	if (file_command_status_ != FS_NONE)
	{
		switch (file_command_status_)
		{
		case AssimpViewer::FS_NONE:
			break;
		//case AssimpViewer::FS_MODEL_LOADED_SUCCESS:
		//	ImGui::OpenPopup("Model Load Success");
		//	break;
		case AssimpViewer::FS_MODEL_LOADED_FAILED:
			ImGui::OpenPopup("Model Load Failed");
			break;

		case AssimpViewer::FS_MODEL_SAVE_DIRECTORY_INVALID:
			ImGui::OpenPopup("Save Directory Invalid. Set Base Directory");
		default:
			break;
		}
	}

	//if (ImGui::BeginPopupModal("Model Load Success", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	//{
	//	if (ImGui::Button("OK", ImVec2(120, 0)))
	//	{
	//		file_command_status_ = FS_NONE;
	//		ImGui::CloseCurrentPopup();
	//	}
	//	ImGui::EndPopup();
	//}

	if (ImGui::BeginPopupModal("Model Load Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			file_command_status_ = FS_NONE;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}


	if (input_manager_)
	{
		input_manager_->Update();

		if (!file_dialog_active_)
		{
			OrbitCamera::Movement camera_movement = OrbitCamera::kNone;

			if (input_manager_->keyboard())
			{
				if (input_manager_->keyboard()->IsKeyDown(camera_key_))
				{
					if (input_manager_->touch_manager()->is_button_down(0))
						camera_movement = OrbitCamera::kRotate;
					else if (input_manager_->touch_manager()->is_button_down(1))
						camera_movement = OrbitCamera::kZoom;
					else if (input_manager_->touch_manager()->is_button_down(2))
						camera_movement = OrbitCamera::kPan;
				}

				if (input_manager_->keyboard()->IsKeyPressed(gef::Keyboard::KC_F))
				{
					// if an animation is playing the frame the skeleton rather than the mesh
					if (anim_player_.clip())
					{
						anim_player_.set_anim_time(0.0f);
						anim_player_.Update(0.0f, bind_pose_);
						FrameSkeleton();
					}
					else
					{
						// frame the mesh
						FrameScene(scene_aabb_);
					}
				}
			}


			if (input_manager_->touch_manager())
			{
				orbit_camera_.Update(frame_time, input_manager_->touch_manager()->mouse_rel(), camera_movement);
				camera_eye_ = orbit_camera_.eye();
				camera_lookat_ = orbit_camera_.lookat();
			}
		}
	}

	return running;
}

void AssimpViewer::Render()
{

	gef::Matrix44 projection_matrix = platform_.PerspectiveProjectionFov(camera_fov_, (float)platform_.width() / (float)platform_.height(), near_plane_, far_plane_);
	gef::Matrix44 view_matrix;

	view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);

	renderer_3d_->set_view_matrix(view_matrix);
	renderer_3d_->set_projection_matrix(projection_matrix);

	renderer_3d_->Begin();

	// draw all the meshes
	if (scene_assets_)
	{
		for (auto mesh : scene_assets_->meshes)
		{
			gef::Matrix44 transform;
			transform.SetIdentity();

			bool mesh_drawn = false;
			if (scene_assets_->skeletons.size() > 0)
			{
				if ((*mesh).vertex_buffer())
				{
					if (mesh->vertex_buffer()->vertex_byte_size() == sizeof(gef::Mesh::SkinnedVertex))
					{
						gef::Skeleton* skeleton = scene_assets_->skeletons.front();
						std::vector<gef::Matrix44>::const_iterator pose_matrix_iter = anim_player_.pose().global_pose().cbegin();
						std::vector<gef::Joint>::const_iterator joint_iter = skeleton->joints().begin();

						for (std::vector<gef::Matrix44>::iterator bone_matrix_iter = bone_matrices_.begin(); bone_matrix_iter != bone_matrices_.end(); ++bone_matrix_iter, ++joint_iter, ++pose_matrix_iter)
							*bone_matrix_iter = (joint_iter->inv_bind_pose * *pose_matrix_iter);

						gef::MeshInstance mesh_instance;
						mesh_instance.set_mesh(mesh);
						renderer_3d_->DrawSkinnedMesh(mesh_instance, bone_matrices_);
						mesh_drawn = true;

						//RenderPose(anim_player_.pose(), 2.0f);
					}
				}
			}

			if (!mesh_drawn)
				renderer_3d_->DrawMesh(*mesh, transform);
		}
	}

	renderer_3d_->End();

	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();

	ImGui::Render();
}


void AssimpViewer::OpenFileDialog(bool open_file_triggered)
{
    if(file_dialog_active_)
    {
		const char* chosenPath = NULL;
		
		if (file_command_ == FC_SAVE_MODEL)
		{
			if (save_directory_.length() == 0)
				save_directory_ = file_dlg_.getLastDirectory();
			chosenPath = file_dlg_.saveFileDialog(open_file_triggered, save_directory_.c_str(), NULL, ".scn");
		}
		else if (file_command_ == FC_LOAD_TEXTURES_DIRECTORY)
		{
			if (load_textures_directory_.length() == 0)
				load_textures_directory_ = file_dlg_.getLastDirectory();
			chosenPath = file_dlg_.chooseFolderDialog(open_file_triggered, load_textures_directory_.c_str());
		}
		else if (file_command_ == FC_SET_BASE_DIRECTORY)
		{
			if (base_save_directory_.length() == 0)
				base_save_directory_ = file_dlg_.getLastDirectory();
			chosenPath = file_dlg_.chooseFolderDialog(open_file_triggered, base_save_directory_.c_str());
		}
		else if(file_command_ == FC_LOAD_ANIMATION)
		{
			if (open_anim_directory_.length() == 0)
				open_anim_directory_ = file_dlg_.getLastDirectory();
			chosenPath = file_dlg_.chooseFileDialog(open_file_triggered, open_anim_directory_.c_str());
		}
		else if (file_command_ == FC_SAVE_ANIMATION)
		{
			if (save_anim_directory_.length() == 0)
				save_anim_directory_ = file_dlg_.getLastDirectory();
			chosenPath = file_dlg_.saveFileDialog(open_file_triggered, save_anim_directory_.c_str(), NULL, ".scn");
		}
		else
			chosenPath = file_dlg_.chooseFileDialog(open_file_triggered, open_directory_.c_str());


        if (strlen(file_dlg_.getChosenPath()) > 0)
        {
            switch(file_command_)
            {

                case FC_NONE:break;
                case FC_LOAD_MODEL:
                {
					open_directory_ = file_dlg_.getLastDirectory();
					load_textures_directory_ = open_directory_;
                    MainMenuBarFileOpen(file_dlg_.getChosenPath(), open_directory_.c_str());
                }
                break;

                case FC_LOAD_TEXTURE:
                {
					LoadSelectedTexture();
                }
                break;

				case FC_SAVE_MODEL:
				{
					save_directory_ = file_dlg_.getLastDirectory();
					MainMenuBarFileSave();
				}
				break;

                case FC_LOAD_TEXTURES_DIRECTORY:
                {
					load_textures_directory_ = file_dlg_.getLastDirectory();
                    MainMenuBarFileLoadTexturesDirectory();
                }
                break;

				case FC_SET_BASE_DIRECTORY:
				{
					MainMenuBarSetBaseDirectory();
				}
				break;

				case FC_LOAD_ANIMATION:
				{
					open_anim_directory_ = file_dlg_.getLastDirectory();
					MainMenuBarFileOpenAnimation(file_dlg_.getChosenPath());
				}
				break;

				case FC_SAVE_ANIMATION:
				{
					save_anim_directory_ = file_dlg_.getLastDirectory();
					MainMenuBarFileSaveAnimation();
				}
				break;

            }

            file_command_ = FC_NONE;
            file_dialog_active_ = false;
        }

		if(file_dlg_.hasUserJustCancelledDialog())
        {
            file_dialog_active_ = false;
        }
    }
}

void AssimpViewer::LoadSelectedTexture()
{
	if (open_texture_num_ != -1)
	{
		auto texture_map = scene_assets_->textures_map.begin();
		advance(texture_map, open_texture_num_);

		open_texture_num_ = -1;

		gef::StringId new_texture_id = scene_assets_->string_id_table.Add(std::string(file_dlg_.getChosenPath()));

		// get old texture filename
		gef::StringId old_texture_id = texture_map->first;
		std::string old_texture_filename;
		scene_assets_->string_id_table.Find(old_texture_id, old_texture_filename);

		// reassign material texture maps from old texture to new one
		std::map<gef::StringId, gef::StringId> old_to_new_texture_filename;

		old_to_new_texture_filename[old_texture_id] = new_texture_id;
		ReassignMaterialTextures(old_to_new_texture_filename);

		// go through all materials and load textures that match the
		// new texture filename
		auto material_data = scene_assets_->material_data.begin();
		for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
		{
			gef::StringId material_texture_id = gef::GetStringId(material_data->diffuse_texture);

			if (material_texture_id == new_texture_id)
				LoadTexture(material_data->diffuse_texture.c_str(), material_num);
			material_data++;
		}
	}
}

void AssimpViewer::LoadTexture(const char *texture_file_path, int material_num)
{
    if (material_num != -1)
    {
        auto material_data = scene_assets_->material_data.begin();
        advance(material_data, material_num);

        auto material = scene_assets_->materials.begin();
        advance(material, material_num);

		// check to see if this texture already exists
		gef::StringId texture_id = scene_assets_->string_id_table.Add(texture_file_path);
        if(scene_assets_->textures_map.find(texture_id) == scene_assets_->textures_map.end())
        {
			gef::Texture *new_texture = CreateTexture(texture_file_path);

			if (new_texture)
			{
				scene_assets_->textures_map[texture_id] = new_texture;

				// add new texture to the list of textures
				scene_assets_->textures.push_back(new_texture);

				// fix up material to point to new material
				(*material)->set_texture(new_texture);
			}
        }
    }
}

void AssimpViewer::LoadTextures(const char *texture_directory)
{
	// delete all textures
	for (auto texture : scene_assets_->textures)
		delete texture;
	scene_assets_->textures.clear();

	for (auto texture_mapping : scene_assets_->textures_map)
	{
		// remove old texture path from string table
		gef::StringId texture_id = texture_mapping.first;
		scene_assets_->string_id_table.Remove(texture_id);
	}
	scene_assets_->textures_map.clear();

	// reset all material textures
	for (auto material : scene_assets_->materials)
		material->set_texture(nullptr);


	for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
	{
		auto material_data = scene_assets_->material_data.begin();
		advance(material_data, material_num);

		std::string src_texture_filename = material_data->diffuse_texture;
		std::string extension;
		std::string texture_filename = ExtractImageFilename(src_texture_filename, extension);
		texture_filename = std::string(texture_directory) + "/" + texture_filename + extension;
		material_data->diffuse_texture = texture_filename;

		LoadTexture(material_data->diffuse_texture.c_str(), material_num);
	}
}

void AssimpViewer::SaveTextures()
{
	// make texture path relative to base directory
	std::string save_directory = file_dlg_.getLastDirectory();
	std::string texture_directory;
	
	if(base_save_directory_.length() < save_directory.length())
		texture_directory = save_directory.substr(base_save_directory_.length() + 1);

	// init DevIL. This needs to be done only once per application
	ilInit();
	int texture_num = 0;
//	std::map<gef::StringId, gef::StringId> old_to_new_texture_filename;
	for (auto& material_data : scene_assets_->material_data)
	{
		std::string src_texture_filename, extension;
		src_texture_filename = material_data.diffuse_texture;

		if (material_data.diffuse_texture.size() > 0)
		{
			std::string texture_filename = ExtractImageFilename(src_texture_filename, extension);
			texture_filename += ".png";


			if(texture_directory.length() > 0)
				texture_filename = texture_directory + "/" + texture_filename;

			// set texture name correctly for saving
			material_data.diffuse_texture = texture_filename;
			scene_assets_->string_id_table.Add(material_data.diffuse_texture);

			unsigned int srcImageID = 0;


			// load source image
			{
				ILboolean success;

				// generate an image name
				ilGenImages(1, &srcImageID);
				// bind it
				ilBindImage(srcImageID);
				// match image origin to OpenGL’s
				ilEnable(IL_ORIGIN_SET);
				ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
				// load  the image
				success = ilLoadImage((ILstring)(src_texture_filename).c_str());
				// check to see if everything went OK
				if (!success)
				{
					ilDeleteImages(1, &srcImageID);
				}
			}

			// save image as png
			if (srcImageID != 0)
			{
				std::string texture_filename_fullpath = base_save_directory_ + "/" + texture_filename;
				ilSaveImage(texture_filename_fullpath.c_str());

				// finished with source image
				ilDeleteImages(1, &srcImageID);
			}

			texture_num++;
		}
	}
	ilShutDown();
//    ReassignMaterialTextures(old_to_new_texture_filename);


}

void AssimpViewer::ReassignMaterialTextures(const std::map<gef::StringId, gef::StringId> &old_to_new_texture_filename) const {// go through all materials and rename textures
    // go through materials and try and load textures from model folder
    auto material_data = scene_assets_->material_data.begin();
    for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
	{
		if (material_data->diffuse_texture.length() > 0)
		{
			gef::StringId old_texture_id = gef::GetStringId(material_data->diffuse_texture.c_str());

            if(old_to_new_texture_filename.find(old_texture_id) != old_to_new_texture_filename.end())
            {
                gef::StringId new_texture_id = old_to_new_texture_filename.at(old_texture_id);

                std::string new_texture_filename;
                if (scene_assets_->string_id_table.Find(new_texture_id, new_texture_filename))
                {
                    gef::Texture* texture = scene_assets_->textures_map[old_texture_id];
                    scene_assets_->textures_map.erase(old_texture_id);
                    scene_assets_->textures_map[new_texture_id] = texture;

                    material_data->diffuse_texture = new_texture_filename;
                }
            }
        }
		material_data++;
	}
}

void AssimpViewer::RenderPose(const gef::SkeletonPose& pose, float size)
{
	for (int bone_num = 0; bone_num < pose.skeleton()->joint_count(); ++bone_num)
	{
		gef::Matrix44 bone_transform = pose.global_pose()[bone_num];

		gef::Matrix44 scale_matrix;
		scale_matrix.Scale(gef::Vector4(size, size, size));
		renderer_3d_->DrawMesh(*primitive_builder_->GetDefaultCubeMesh(), scale_matrix*bone_transform);
	}
}

std::string AssimpViewer::ExtractImageFilename(std::string &src_filename, std::string &extension)
{
	bool extension_processed = false;
	std::string filename;
    for (auto filename_char = src_filename.rbegin(); filename_char != src_filename.rend(); filename_char++)
	{
		if (!extension_processed)
		{
			extension = std::string(1, *filename_char) + extension;

			if (*filename_char == '.')
				extension_processed = true;
		}
		else
		{
			if (*filename_char == '\\' || *filename_char == '/')
				break;

			filename = std::string(1, *filename_char) + filename;
		}
	}
	return filename;
}

void AssimpViewer::MainMenuBarFileOpen(const char* filepath, const char* texture_directory)
{

	gef::Scene* new_assets = new gef::Scene();
	bool import_success = loader_->ReadAssets(filepath, new_assets, &platform_);

	if (import_success)
	{
		// reset pose in the anim player
		anim_player_.set_clip(nullptr);
		anim_player_.Update(0.0f, gef::SkeletonPose());

		// delete previously loaded assets
		delete scene_assets_;
		scene_assets_ = new_assets;

		// check to see if there is a skeleton in the the scene file
		// if so, pull out the bind pose and create an array of matrices
		// that will be used to store the bone transformations
		if (scene_assets_->skeletons.size() > 0)
		{
			gef::Skeleton* skeleton = scene_assets_->skeletons.front();
			bind_pose_.CreateBindPose(skeleton);
			bone_matrices_.resize(skeleton->joints().size());
			anim_player_.Update(0.0f, bind_pose_);
		}


		scene_assets_->CreateMaterials(platform_);

		// go through materials and try and load textures from model folder
		auto material_data = scene_assets_->material_data.begin();
		for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
		{
			open_material_num_ = material_num;
			if (material_data->diffuse_texture.length() > 0)
			{
				std::string texture_filepath = texture_directory + std::string("/") + material_data->diffuse_texture;
                LoadTexture(texture_filepath.c_str(), open_material_num_);
                open_material_num_ = -1;
			}
			material_data++;
		}

		scene_assets_->CreateMeshes(platform_);

		scene_aabb_ = loader_->get_scene_aabb();
		FrameScene(scene_aabb_);

		file_command_status_ = FS_MODEL_LOADED_SUCCESS;
	}
	else
	{
		// import failed, throw away new assets
		delete new_assets;
		new_assets = nullptr;

		file_command_status_ = FS_MODEL_LOADED_FAILED;
	}
}

void AssimpViewer::MainMenuBarFileSave()
{
	if (scene_assets_)
	{
		std::string save_file_path = file_dlg_.getChosenPath();


		if (base_save_directory_.length() == 0)
			base_save_directory_ = save_directory_;

		// make sure file path is under the base directory
		if (save_file_path.compare(0, base_save_directory_.size(), base_save_directory_) == 0)
		{
			// save texture paths as they will be overwritten when saving
			std::list<std::string> previous_texture_paths;
			for (auto material_data : scene_assets_->material_data)
			{
				previous_texture_paths.push_back(material_data.diffuse_texture);

				// remove previous texture path from string table
				scene_assets_->string_id_table.Remove(gef::GetStringId(material_data.diffuse_texture));
			}


			// saving textures must be done before the scene is written as it overwrites texture filenames
			SaveTextures();

			scene_assets_->WriteSceneToFile(platform_, save_file_path.c_str());

			// restore material texture paths
			auto previous_texture_path = previous_texture_paths.begin();
			for (auto material_data : scene_assets_->material_data)
			{
				scene_assets_->string_id_table.Remove(gef::GetStringId(material_data.diffuse_texture));
				material_data.diffuse_texture = *previous_texture_path;
				scene_assets_->string_id_table.Add(material_data.diffuse_texture);
			}
		}
		else
		{
			file_command_status_ = FS_MODEL_SAVE_DIRECTORY_INVALID;
		}
	}
}

void AssimpViewer::MainMenuBarFileLoadTexturesDirectory()
{
    if(scene_assets_)
    {
        const char* textures_directory = load_textures_directory_.c_str();
        LoadTextures(textures_directory);
    }
}

void AssimpViewer::MainMenuBarSetBaseDirectory()
{
	base_save_directory_ = file_dlg_.getLastDirectory();
}

void AssimpViewer::MainMenuBarFileOpenAnimation(const char* filepath)
{

	gef::Scene* new_assets = new gef::Scene();
	bool import_success = loader_->ReadAnimation(filepath, new_assets);

	if (import_success)
	{
		// delete previously loaded assets
		delete anim_assets_;
		anim_assets_ = nullptr;
		anim_assets_ = new_assets;

		if (anim_assets_->animations.size() > 0)
		{
			anim_player_.set_clip(anim_assets_->animations.cbegin()->second);
			anim_player_.set_looping(true);
			anim_player_.set_anim_time(0.0f);
			anim_player_.Update(0.0f, bind_pose_);

			FrameSkeleton();

		}

		file_command_status_ = FS_ANIMATION_LOADED_SUCCESS;
	}
	else
	{
		// import failed, throw away new assets
		delete new_assets;
		new_assets = nullptr;

		file_command_status_ = FS_ANIMATION_LOADED_FAILED;
	}

}

void AssimpViewer::FrameSkeleton()
{
	// calculate skeleton AABB from joint positions
	gef::Aabb anim_aabb;

	for (int joint_num = 0; joint_num < anim_player_.pose().global_pose().size(); ++joint_num)
	{
		const gef::Matrix44& joint_world_transform = anim_player_.pose().global_pose()[joint_num];
		anim_aabb.Update(joint_world_transform.GetTranslation());
	}

	skeleton_aabb_ = anim_aabb;
	FrameScene(anim_aabb);
}

void AssimpViewer::MainMenuBarFileSaveAnimation()
{
	if (anim_assets_)
	{
		std::string save_file_path = file_dlg_.getChosenPath();
		if (base_save_directory_.length() == 0)
			base_save_directory_ = save_anim_directory_;

		// make sure file path is under the base directory
		if (save_file_path.compare(0, base_save_directory_.size(), base_save_directory_) == 0)
			anim_assets_->WriteSceneToFile(platform_, save_file_path.c_str());
		else
			file_command_status_ = FS_MODEL_SAVE_DIRECTORY_INVALID;
	}
}

void AssimpViewer::MainMenuBar(bool &running, bool &file_command_triggered)
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
			bool save_options_active = base_save_directory_.size() > 0;
			bool save_animations_active = false;
			
			if(anim_assets_)
				save_animations_active = anim_assets_->animations.size() > 0;

			AddFileMenuItem(file_command_triggered, "Open", "Ctrl+O", FC_LOAD_MODEL);
			AddFileMenuItem(file_command_triggered, "Save", "Ctrl+S", FC_SAVE_MODEL, save_options_active);
			ImGui::Separator();
			AddFileMenuItem(file_command_triggered, "Open Animation", nullptr, FC_LOAD_ANIMATION);
			AddFileMenuItem(file_command_triggered, "Save Animation", nullptr, FC_SAVE_ANIMATION, save_animations_active);
			ImGui::Separator();
			AddFileMenuItem(file_command_triggered, "Load Textures Directory", nullptr, FC_LOAD_TEXTURES_DIRECTORY);
			AddFileMenuItem(file_command_triggered, "Set Save Base Directory", nullptr, FC_SET_BASE_DIRECTORY);
			ImGui::Separator();

            if(ImGui::MenuItem("Exit"))
            {
                running = false;
            }


            ImGui::EndMenu();
        }

        //if(ImGui::BeginMenu("Window"))
        //{
        //    ImGui::Checkbox("Textures", &textures_window_visible_);
        //    ImGui::EndMenu();
        //}

        ImGui::EndMainMenuBar();
    }
}

bool AssimpViewer::AddFileMenuItem(bool& file_command_triggered, const char* item_text, const char* shortcut, FILE_COMMAND file_command, bool enabled)
{
	bool item_triggered = ImGui::MenuItem(item_text, shortcut, file_dialog_active_, !file_dialog_active_ && enabled);
	if (item_triggered)
	{
		if (!file_dialog_active_)
		{
			file_command_triggered = item_triggered;
			file_dialog_active_ = true;
			file_command_ = file_command;
		}
	}
	return file_command_triggered;
}

void AssimpViewer::MeshesMenu() const
{
    ImGui::Begin("Meshes");

    auto mesh_data = scene_assets_->mesh_data.begin();
    for (size_t mesh_num = 0; mesh_num < scene_assets_->mesh_data.size(); ++mesh_num)
    {
        std::string mesh_text = "Mesh: " + std::to_string(mesh_num);
        ImGui::Text("%s", mesh_text.c_str());
        mesh_data++;
    }

    ImGui::End();
}

void AssimpViewer::ModelMenu() const
{
	ImGui::Begin("Model");

	if (scene_assets_->mesh_data.size() > 0)
	{
		int index_count = 0;

		for (auto primitive : scene_assets_->mesh_data.begin()->primitives)
		{
			index_count += primitive->num_indices;
		}

		ImGui::Text("Tri count: %d", index_count / 3);
	}


	ImGui::Text("Mesh AABB min: (%.2f, %.2f, %.2f)", scene_aabb_.min_vtx().x(), scene_aabb_.min_vtx().y(), scene_aabb_.min_vtx().z());
	ImGui::Text("Mesh AABB max: (%.2f, %.2f, %.2f)", scene_aabb_.max_vtx().x(), scene_aabb_.max_vtx().y(), scene_aabb_.max_vtx().z());

	if (scene_assets_->skeletons.size() > 0)
	{
		ImGui::Text("Bone count: %d", scene_assets_->skeletons.front()->joint_count());
	}

	if (anim_player_.clip())
	{
		ImGui::Text("Anim AABB min: (%.2f, %.2f, %.2f)", skeleton_aabb_.min_vtx().x(), skeleton_aabb_.min_vtx().y(), skeleton_aabb_.min_vtx().z());
		ImGui::Text("Anim AABB max: (%.2f, %.2f, %.2f)", skeleton_aabb_.max_vtx().x(), skeleton_aabb_.max_vtx().y(), skeleton_aabb_.max_vtx().z());
	}

	//	ImGui::Text("camera eye: (%.2f, %.2f, %.2f)", camera_eye_.x(), camera_eye_.y(), camera_eye_.z());
//	ImGui::Text("camera lookat: (%.2f, %.2f, %.2f)", camera_lookat_.x(), camera_lookat_.y(), camera_lookat_.z());

	ImGui::End();
}

void AssimpViewer::LoadOptionsMenu() const
{
	ImGui::Begin("Load Options");

	bool flip_winding_order = loader_->flip_winding_order();
	if (ImGui::Checkbox("Flip winding order", &flip_winding_order))
		loader_->set_flip_winding_order(flip_winding_order);

	bool make_left_handed = loader_->make_left_handed();
	if (ImGui::Checkbox("Make left handed", &make_left_handed))
		loader_->set_make_left_handed(make_left_handed);

    bool generate_normals = loader_->generate_normals();
    if (ImGui::Checkbox("Generate normals", &generate_normals))
        loader_->set_generate_normals(generate_normals);

    bool rotate_90_xaxis = loader_->rotate_90_xaxis();
    if (ImGui::Checkbox("Fix Z-up", &rotate_90_xaxis))
        loader_->set_rotate_90_xaxis(rotate_90_xaxis);

	bool ignore_skinning = loader_->ignore_skinning();
	if (ImGui::Checkbox("Ignore skinning data", &ignore_skinning))
		loader_->set_ignore_skinning(ignore_skinning);

	ImGui::End();
}

void AssimpViewer::DirectoryPopup() const
{
	ImGui::Begin("Directories");

	ImGui::Text((std::string("Load Texture Directory: ") + load_textures_directory_).c_str());

	if(base_save_directory_.size() > 0)
		ImGui::Text((std::string("Save Base Directory: ") + base_save_directory_).c_str());
	else
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Save Base Directory: NOT SET");

	ImGui::End();
}

void AssimpViewer::MaterialsMenu(bool& open_file_triggered)
{
    ImGui::Begin("Materials");

    auto material_data = scene_assets_->material_data.begin();

    for(size_t material_num=0; material_num < scene_assets_->material_data.size(); ++material_num)
    {
        ImGui::PushID(material_num);
        std::string material_name;
        scene_assets_->string_id_table.Find(material_data->name_id, material_name);

        ImGui::Text("%s", material_name.c_str());

        std::string texture_filepath = material_data->diffuse_texture;


        std::string texture_text = "Texture: ";

        if(texture_filepath.length() > 0)
        {
            texture_text += texture_filepath;

            ImGui::Text("%s", texture_text.c_str());
            bool texture_button_triggered = ImGui::Button("...");

            if(texture_button_triggered)
            {
                if(!file_dialog_active_)
                {
                    open_file_triggered = texture_button_triggered;
                    file_dialog_active_ = true;
                    file_command_ = FC_LOAD_TEXTURE;
                    open_material_num_ = material_num;
                }
            }
        }
        else
        {
            texture_text += "None";
            ImGui::Text("%s", texture_text.c_str());
        }


        ImGui::Separator();
        ImGui::PopID();
        material_data++;
    }

    ImGui::End();
}


void AssimpViewer::TexturesMenu(bool& open_file_triggered)
{
    ImGui::Begin("Textures");

    int texture_num = 0;
    for (auto texture_mapping : scene_assets_->textures_map)
    {
        ImGui::PushID(texture_num);
        std::string texture_filepath;
        scene_assets_->string_id_table.Find(texture_mapping.first, texture_filepath);

        std::string texture_text = "Texture: ";

        if(texture_filepath.length() > 0)
        {
            texture_text += texture_filepath;

            ImGui::Text("%s", texture_text.c_str());
            bool texture_button_triggered = ImGui::Button("...");

            if(texture_button_triggered)
            {
                if(!file_dialog_active_)
                {
                    open_file_triggered = texture_button_triggered;
                    file_dialog_active_ = true;
                    file_command_ = FC_LOAD_TEXTURE;
                    open_texture_num_ = texture_num;
                }
            }
        }
        else
        {
            texture_text += "None";
            ImGui::Text("%s", texture_text.c_str());
        }


        ImGui::Separator();
        ImGui::PopID();
        texture_num++;
    }

    ImGui::End();
}

void AssimpViewer::InitFont()
{
	font_ = new gef::Font(platform_);
	font_->Load("comic_sans");
}

void AssimpViewer::CleanUpFont()
{
	delete font_;
	font_ = nullptr;
}

void AssimpViewer::DrawHUD()
{
	if(font_)
	{
		// display frame rate
		font_->RenderText(sprite_renderer_, gef::Vector4(1170.0f, 636.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "anim: %.2f", anim_player_.anim_time());


		font_->RenderText(sprite_renderer_, gef::Vector4(1170.0f, 686.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void AssimpViewer::SetupLights()
{
    gef::PointLight default_point_light;
    default_point_light.set_colour(gef::Colour(1.0f, 1.0f, 1.0f, 1.0f));
    default_point_light.set_position(gef::Vector4(0.0f, 400.0f, 700.0f));

    gef::PointLight back_point_light;
    back_point_light.set_colour(gef::Colour(1.0f, 1.0f, 1.0f, 1.0f));
    back_point_light.set_position(gef::Vector4(0.0f, 400.0f, -700.0f));

    gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
    default_shader_data.set_ambient_light_colour(gef::Colour(0.0f, 0.0f, 0.0f, 1.0f));
    default_shader_data.AddPointLight(default_point_light);
    default_shader_data.AddPointLight(back_point_light);
}

void AssimpViewer::SetupCamera()
{
    // initialise the camera settings
    camera_eye_ = gef::Vector4(5.0f, 5.0f, 5.0f);
    camera_lookat_ = gef::Vector4(0.0f, 0.0f, 0.0f);
    camera_up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
    camera_fov_ = gef::DegToRad(45.0f);
    near_plane_ = 0.01f;
    far_plane_ = 1000.f;
}

void AssimpViewer::FrameScene(const gef::Aabb& aabb)
{
    // look at the centre of the scene
    gef::Vector4 scene_centre = (aabb.max_vtx() + aabb.min_vtx())*0.5f;
    camera_lookat_ = scene_centre;
    camera_eye_.set_x(camera_lookat_.x());
    camera_eye_.set_y(camera_lookat_.y());

    // figure out how far to move the camera back so we can frame the scene

    const float view_coverage_percentage = 0.75f;
    const float camera_half_fov = camera_fov_*0.5f;

    gef::Vector4 scene_aabb_size = aabb.max_vtx() - aabb.min_vtx();
    float camera_distance = 0.0f;
    float aspect_ratio = (float)platform_.width() / (float)platform_.height();

    // height
    {
        float height = (scene_aabb_size.y()*0.5f) / view_coverage_percentage;

        float zoom_distance = height / tanf(camera_half_fov);


        if(zoom_distance > camera_distance)
            camera_distance = zoom_distance;
    }

    // width
    {
        float width = (scene_aabb_size.x()*0.5f) / view_coverage_percentage;

        float zoom_distance = width / (tanf(camera_half_fov)*aspect_ratio);


        if(zoom_distance > camera_distance)
            camera_distance = zoom_distance;
    }

    // depth
    {
        float zoom_distance = (scene_aabb_size.z()*0.5f) / view_coverage_percentage;

        if(zoom_distance > camera_distance)
            camera_distance = zoom_distance;
    }

    camera_eye_.set_z(camera_lookat_.z()+scene_aabb_size.z()*0.5f+camera_distance);

//	camera_eye_ *= 0.01f;
//	camera_lookat_ *= 0.01f;

    orbit_camera_.set_eye(camera_eye_);
    orbit_camera_.set_lookat(camera_lookat_);
	orbit_camera_.set_up(camera_up_);
}

gef::Texture *AssimpViewer::CreateTexture(const std::string &image_filename)
{
    gef::Texture* texture = nullptr;

    ILboolean success;
    unsigned int imageID;

    // init DevIL. This needs to be done only once per application
    ilInit();
    // generate an image name
    ilGenImages(1, &imageID);
    // bind it
    ilBindImage(imageID);
    // match image origin to OpenGL’s
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
    // load  the image
    success = ilLoadImage((ILstring)image_filename.c_str());
    // check to see if everything went OK
    if (!success)
    {
        ilDeleteImages(1, &imageID);
    }
    else
    {
        gef::ImageData image_data;

        image_data.set_width(ilGetInteger(IL_IMAGE_WIDTH));
        image_data.set_height(ilGetInteger(IL_IMAGE_HEIGHT));

        // Convert image to RGBA with unsigned byte data type
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

        image_data.set_image(ilGetData());


        texture = gef::Texture::Create(platform_, image_data);

        // clear image data here as il library will clean up image data
        image_data.set_image(nullptr);


        ilDeleteImages(1, &imageID);
    }

    ilShutDown();

    return texture;
}
