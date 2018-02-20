#ifndef _ASSIMP_VIEWER_H
#define _ASSIMP_VIEWER_H
#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>

#include <imgui.h>
#include <addons/imguifilesystem/imguifilesystem.h>
#include <graphics/scene.h>
#include "orbit_camera.h"

// FRAMEWORK FORWARD DECLARATIONS
namespace gef
{
	class Platform;
	class SpriteRenderer;
	class Font;
	class Renderer3D;
	class Mesh;
    class Scene;
    class InputManager;
}

class AssimpSceneLoader;

class AssimpViewer : public gef::Application
{
    enum FILE_COMMAND
    {
        FC_NONE,
        FC_LOAD_MODEL,
		FC_LOAD_TEXTURE,
		FC_SAVE_MODEL,
	};

	enum FILE_COMMAND_STATUS
	{
		FS_NONE,
		FS_MODEL_LOADED_SUCCESS,
		FS_MODEL_LOADED_FAILED,
	};

public:
	AssimpViewer(gef::Platform& platform);
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
    void FrameScene();


	gef::Renderer3D* renderer_3d_;

	gef::SpriteRenderer* sprite_renderer_;
    gef::InputManager* input_manager_;
	gef::Font* font_;

	float fps_;

    gef::Vector4 camera_eye_;
    gef::Vector4 camera_lookat_;
    gef::Vector4 camera_up_;
    float camera_fov_;
    float near_plane_;
    float far_plane_;

    ImGuiFs::Dialog open_file_dlg_;
    bool open_file_dialog_active_;

    gef::Scene* scene_assets_;
    gef::Aabb scene_aabb_;
    OrbitCamera orbit_camera_;

    FILE_COMMAND file_command_;
	FILE_COMMAND_STATUS file_command_status_;
    int open_material_num_;

	AssimpSceneLoader* loader_;


    void MaterialsMenu(bool& open_file_triggered);

	void MeshesMenu() const;

	void ModelMenu() const;

	void LoadOptionsMenu() const;

	gef::Texture* CreateTexture(const std::string& texture_filepath);

    void MainMenuBar(bool &running, bool &open_file_triggered);

    void OpenFileDialog(bool open_file_triggered);

	void MainMenuBarFileOpen();
	void MainMenuBarFileSave();

    void LoadTexture(const char *texture_file_path);

	void SaveTextures();

	std::string ExtractImageFilename(std::string &src_filename);

};

#endif // _ASSIMP_VIEWER_H
