#include "assimp_viewer.h"
#include <system/platform.h>
#include <graphics/sprite_renderer.h>
#include <graphics/texture.h>
#include <graphics/material.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <graphics/font.h>
#include <input/input_manager.h>
#include <input/touch_input_manager.h>
#include <input/keyboard.h>
#include <maths/math_utils.h>
#include <graphics/renderer_3d.h>
#include "assimp_scene_loader.h"



#include <IL/il.h>

void ImGui_NewFrame();

AssimpViewer::AssimpViewer(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(nullptr),
    renderer_3d_(nullptr),
    input_manager_(nullptr),
	font_(nullptr),
    open_file_dialog_active_(false),
    scene_assets_(nullptr),
    file_command_(FC_NONE),
    open_material_num_(-1),
	file_command_status_(FS_NONE),
	loader_(nullptr)
{
}

void AssimpViewer::Init()
{
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
    renderer_3d_ = gef::Renderer3D::Create(platform_);

    input_manager_ = gef::InputManager::Create(platform_);

    // make sure if there is a panel to detect touch input, then activate it
    if (input_manager_ && input_manager_->touch_manager() && (input_manager_->touch_manager()->max_num_panels() > 0))
        input_manager_->touch_manager()->EnablePanel(0);


	InitFont();

    SetupCamera();
    SetupLights();

	loader_ = new AssimpSceneLoader();
}

void AssimpViewer::CleanUp()
{
	delete loader_;
	loader_ = nullptr;

    delete scene_assets_;
    scene_assets_ = nullptr;

	CleanUpFont();
	delete sprite_renderer_;
	sprite_renderer_ = nullptr;

    delete renderer_3d_;
    renderer_3d_ = nullptr;
}

bool AssimpViewer::Update(float frame_time)
{
	fps_ = 1.0f / frame_time;

    if(input_manager_)
    {
        input_manager_->Update();


        OrbitCamera::Movement camera_movement = OrbitCamera::kNone;

        if(input_manager_->keyboard() && input_manager_->touch_manager())
        {
            if (input_manager_->keyboard()->IsKeyDown(gef::Keyboard::KC_LCONTROL))
            {
                if(input_manager_->touch_manager()->is_button_down(0))
                    camera_movement = OrbitCamera::kRotate;
                else if(input_manager_->touch_manager()->is_button_down(1))
                    camera_movement = OrbitCamera::kZoom;
                else if(input_manager_->touch_manager()->is_button_down(2))
                    camera_movement = OrbitCamera::kPan;
            }
        }


        if(input_manager_->touch_manager())
        {
            orbit_camera_.Update(frame_time, input_manager_->touch_manager()->mouse_rel(), camera_movement);
            camera_eye_ = orbit_camera_.eye();
            camera_lookat_ = orbit_camera_.lookat();
        }
    }

    bool running = true;
    bool open_file_triggered = false;

    ImGui_NewFrame();

    MainMenuBar(running, open_file_triggered);

	LoadOptionsMenu();

	
	// if we have an object loaded, enable object UI
    if(scene_assets_ && (scene_assets_->meshes.size() > 0))
    {
		ModelMenu();
        MaterialsMenu(open_file_triggered);
        // MeshesMenu();

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


	return running;
}

void AssimpViewer::OpenFileDialog(bool open_file_triggered)
{
    if(open_file_dialog_active_)
    {
		const char* chosenPath = NULL;
		
		if (file_command_ == FC_SAVE_MODEL)
			chosenPath = open_file_dlg_.saveFileDialog(open_file_triggered, open_file_dlg_.getLastDirectory(), NULL, ".scn");
		else
			chosenPath = open_file_dlg_.chooseFileDialog(open_file_triggered, open_file_dlg_.getLastDirectory());             // see other dialog types and the full list of arguments for advanced usage

        if (strlen(open_file_dlg_.getChosenPath()) > 0)
        {
            switch(file_command_)
            {

                case FC_NONE:break;
                case FC_LOAD_MODEL:
                {
                    MainMenuBarFileOpen();
                }
                break;

                case FC_LOAD_TEXTURE:
                {
                    LoadTexture(open_file_dlg_.getChosenPath());
                }
                break;

				case FC_SAVE_MODEL:
				{
					MainMenuBarFileSave();
				}
				break;

            }

            file_command_ = FC_NONE;
            open_file_dialog_active_ = false;
        }

		if(open_file_dlg_.hasUserJustCancelledDialog())
        {
            open_file_dialog_active_ = false;
        }
    }
}

void AssimpViewer::LoadTexture(const char *texture_file_path)
{
    if (open_material_num_ != -1)
    {
        gef::Texture *new_texture = CreateTexture(texture_file_path);
        if (new_texture)
        {
            auto material_data = scene_assets_->material_data.begin();
            advance(material_data, open_material_num_);

            auto material = scene_assets_->materials.begin();
            advance(material, open_material_num_);

            // only remove previous texture if there is one
            if ((*material)->texture())
            {
                gef::StringId old_texture_id = gef::GetStringId(material_data->diffuse_texture);
                gef::Texture *old_texture = scene_assets_->textures_map[old_texture_id];
                // remove old texture from the list
                scene_assets_->textures.remove(old_texture);
                scene_assets_->textures_map.erase(old_texture_id);
            }

            // map material data to new texture
            material_data->diffuse_texture = texture_file_path;

            // add new texture to the list of textures
            scene_assets_->textures.push_back(new_texture);

            gef::StringId new_texture_id = scene_assets_->string_id_table.Add(material_data->diffuse_texture);
            scene_assets_->textures_map[new_texture_id] = new_texture;
			

            // fix up material to point to new material
            (*material)->set_texture(new_texture);
        }
    }

    open_material_num_ = -1;
}

void AssimpViewer::SaveTextures()
{
	// init DevIL. This needs to be done only once per application
	ilInit();
	int texture_num = 0;
	std::map<gef::StringId, gef::StringId> old_to_new_texture_filename;
	for (auto texture_mapping : scene_assets_->textures_map)
	{
		std::string src_texture_filename;
		gef::StringId src_texture_filename_id;
		src_texture_filename_id = texture_mapping.first;

		if (scene_assets_->string_id_table.Find(src_texture_filename_id, src_texture_filename))
		{
			std::string texture_filename = ExtractImageFilename(src_texture_filename);
			texture_filename += ".png";


			gef::StringId new_texture_filename_id = scene_assets_->string_id_table.Add(texture_filename.c_str());
			old_to_new_texture_filename[src_texture_filename_id] = new_texture_filename_id;
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
				std::string texture_filename_fullpath = open_file_dlg_.getLastDirectory() + std::string("/") + texture_filename;
				ilSaveImage(texture_filename_fullpath.c_str());

				// finished with source image
				ilDeleteImages(1, &srcImageID);
			}

			texture_num++;
		}
	}
	ilShutDown();

	// go through all materials and rename textures
	// go through materials and try and load textures from model folder
	auto material_data = scene_assets_->material_data.begin();
	for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
	{
		if (material_data->diffuse_texture.length() > 0)
		{
			gef::StringId old_texture_id = gef::GetStringId(material_data->diffuse_texture.c_str());
			gef::StringId new_texture_id = old_to_new_texture_filename[old_texture_id];

			std::string new_texture_filename;
			if (scene_assets_->string_id_table.Find(new_texture_id, new_texture_filename))
			{
				gef::Texture* texture = scene_assets_->textures_map[old_texture_id];
				scene_assets_->textures_map.erase(old_texture_id);
				scene_assets_->textures_map[new_texture_id] = texture;

				material_data->diffuse_texture = new_texture_filename;
			}
		}
		material_data++;
	}


}

std::string AssimpViewer::ExtractImageFilename(std::string &src_filename)
{
	bool extension_processed = false;
	std::string filename;
	std::string extension;
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

void AssimpViewer::MainMenuBarFileOpen()
{

	gef::Scene* new_assets = new gef::Scene();
	bool import_success = loader_->ReadAssets(open_file_dlg_.getChosenPath(), new_assets, &platform_);

	if (import_success)
	{
		// delete previously loaded assets
		delete scene_assets_;
		scene_assets_ = nullptr;

		scene_assets_ = new_assets;
		scene_assets_->CreateMaterials(platform_);

		// go through materials and try and load textures from model folder
		auto material_data = scene_assets_->material_data.begin();
		for (size_t material_num = 0; material_num < scene_assets_->material_data.size(); ++material_num)
		{
			open_material_num_ = material_num;
			if (material_data->diffuse_texture.length() > 0)
			{
				std::string texture_filepath = open_file_dlg_.getLastDirectory() + std::string("/") + material_data->diffuse_texture;
				LoadTexture(texture_filepath.c_str());
			}
			material_data++;
		}

		scene_assets_->CreateMeshes(platform_);

		scene_aabb_ = loader_->get_scene_aabb();
		FrameScene();

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
		const char* save_file_path = open_file_dlg_.getChosenPath();

		// saving textures must be done before the scene is written as it overwrites texture filenames
		SaveTextures();

		scene_assets_->WriteSceneToFile(platform_, save_file_path);
	}
}

void AssimpViewer::MainMenuBar(bool &running, bool &open_file_triggered)
{
    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            bool open_triggered = ImGui::MenuItem("Open", "Ctrl+O", open_file_dialog_active_, !open_file_dialog_active_);

            if(open_triggered)
            {
                if(!open_file_dialog_active_)
                {
                    open_file_triggered = open_triggered;
                    open_file_dialog_active_ = true;
                    file_command_ = FC_LOAD_MODEL;
                }
            }


			bool save_triggered = ImGui::MenuItem("Save", "Ctrl+S", open_file_dialog_active_, !open_file_dialog_active_);
			if (save_triggered)
			{
				if (!open_file_dialog_active_)
				{
					open_file_triggered = save_triggered;
					open_file_dialog_active_ = true;
					file_command_ = FC_SAVE_MODEL;
				}
			}

            if(ImGui::MenuItem("Exit"))
            {
                running = false;
            }


            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
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

	ImGui::Text("AABB min: (%.2f, %.2f, %.2f)", scene_aabb_.min_vtx().x(), scene_aabb_.min_vtx().y(), scene_aabb_.min_vtx().z());
	ImGui::Text("AABB max: (%.2f, %.2f, %.2f)", scene_aabb_.max_vtx().x(), scene_aabb_.max_vtx().y(), scene_aabb_.max_vtx().z());
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
                if(!open_file_dialog_active_)
                {
                    open_file_triggered = texture_button_triggered;
                    open_file_dialog_active_ = true;
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

void AssimpViewer::Render()
{

    gef::Matrix44 projection_matrix = platform_.PerspectiveProjectionFov(camera_fov_, (float)platform_.width() / (float)platform_.height(), near_plane_, far_plane_);
    gef::Matrix44 view_matrix;

    view_matrix.LookAt(camera_eye_, camera_lookat_, camera_up_);

    renderer_3d_->set_view_matrix(view_matrix);
    renderer_3d_->set_projection_matrix(projection_matrix);

    renderer_3d_->Begin();

    // draw all the meshes
    if(scene_assets_)
    {
        for(auto mesh : scene_assets_->meshes)
        {
            gef::Matrix44 transform;
            transform.SetIdentity();

            renderer_3d_->DrawMesh(*mesh, transform);
        }
    }

    renderer_3d_->End();

	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();

    ImGui::Render();
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
		font_->RenderText(sprite_renderer_, gef::Vector4(1170.0f, 686.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, "FPS: %.1f", fps_);
	}
}

void AssimpViewer::SetupLights()
{
    gef::PointLight default_point_light;
    default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
    default_point_light.set_position(gef::Vector4(-500.0f, 400.0f, 700.0f));

    gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
    default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
    default_shader_data.AddPointLight(default_point_light);
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

void AssimpViewer::FrameScene()
{
    // look at the centre of the scene
    gef::Vector4 scene_centre = (scene_aabb_.max_vtx() + scene_aabb_.min_vtx())*0.5f;
    camera_lookat_ = scene_centre;
    camera_eye_.set_x(camera_lookat_.x());
    camera_eye_.set_y(camera_lookat_.y());

    camera_lookat_.set_z(camera_lookat_.y());

    // figure out how far to move the camera back so we can frame the scene

    const float view_coverage_percentage = 0.75f;
    const float camera_half_fov = camera_fov_*0.5f;

    gef::Vector4 scene_aabb_size = scene_aabb_.max_vtx() - scene_aabb_.min_vtx();
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

    orbit_camera_.set_eye(camera_eye_);
    orbit_camera_.set_lookat(camera_lookat_);
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
