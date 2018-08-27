#ifndef _ASSIMP_VIEWER_H
#define _ASSIMP_VIEWER_H
#include <system/application.h>
#include <graphics/sprite.h>
#include <maths/vector2.h>
#include <maths/vector4.h>
#include <vector>
#include <graphics/mesh_instance.h>
#include <animation/skeleton.h>

#include <imgui.h>
#include <addons/imguifilesystem/imguifilesystem.h>
#include <graphics/scene.h>
#include <input/keyboard.h>
#include "orbit_camera.h"
#include "motion_clip_player.h"

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
class PrimitiveBuilder;

class AssimpViewer : public gef::Application
{
    enum FILE_COMMAND
    {
        FC_NONE,
        FC_LOAD_MODEL,
		FC_LOAD_TEXTURE,
		FC_SAVE_MODEL,
		FC_LOAD_TEXTURES_DIRECTORY,
		FC_SET_BASE_DIRECTORY,
		FC_LOAD_ANIMATION,
		FC_SAVE_ANIMATION
	};

	enum FILE_COMMAND_STATUS
	{
		FS_NONE,
		FS_MODEL_LOADED_SUCCESS,
		FS_MODEL_LOADED_FAILED,
		FS_MODEL_SAVE_DIRECTORY_INVALID,
		FS_ANIMATION_LOADED_SUCCESS,
		FS_ANIMATION_LOADED_FAILED,
	};

public:
	AssimpViewer(gef::Platform& platform);
	void Init();
	void CleanUp();
	bool Update(float frame_time);
	void Render();
	inline gef::Keyboard::KeyCode camera_key() const { return camera_key_; }
	inline void set_camera_key(gef::Keyboard::KeyCode val) { camera_key_ = val; }
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

    ImGuiFs::Dialog file_dlg_;
    bool file_dialog_active_;

	gef::Scene* scene_assets_;
	gef::Scene* anim_assets_;
	gef::Aabb scene_aabb_;
    OrbitCamera orbit_camera_;

    FILE_COMMAND file_command_;
	FILE_COMMAND_STATUS file_command_status_;
    int open_material_num_;
    int open_texture_num_;

	AssimpSceneLoader* loader_;
    bool textures_window_visible_;
	gef::Keyboard::KeyCode camera_key_;

	std::string open_directory_;
	std::string save_directory_;
	std::string load_textures_directory_;
	std::string base_save_directory_;
	std::string open_anim_directory_;
	std::string save_anim_directory_;


	//const gef::Skeleton* skeleton_;
	std::vector<gef::Matrix44> bone_matrices_;
	gef::SkeletonPose bind_pose_;
	gef::SkeletonPose anim_pose_;
	MotionClipPlayer anim_player_;

	PrimitiveBuilder* primitive_builder_;


    void MaterialsMenu(bool& open_file_triggered);
    void TexturesMenu(bool& open_file_triggered);

	void MeshesMenu() const;

	void ModelMenu() const;

	void LoadOptionsMenu() const;

	gef::Texture* CreateTexture(const std::string& texture_filepath);

    void MainMenuBar(bool &running, bool &file_command_triggered);

	bool AddFileMenuItem(bool& file_command_triggered, const char* item_text, const char* shortcut, FILE_COMMAND file_command);

    void OpenFileDialog(bool open_file_triggered);

	void LoadSelectedTexture();

	void MainMenuBarFileOpen(const char* filepath, const char* texture_directory);
	void MainMenuBarFileSave();
	void MainMenuBarFileLoadTexturesDirectory();
	void MainMenuBarSetBaseDirectory();
	void MainMenuBarFileOpenAnimation(const char* filepath);
	void MainMenuBarFileSaveAnimation();

    void LoadTexture(const char *texture_file_path, int material_num);

	void LoadTextures(const char *texture_directory);
	void SaveTextures();

	std::string ExtractImageFilename(std::string &src_filename, std::string &extension);

    void ReassignMaterialTextures(const std::map<gef::StringId, gef::StringId> &old_to_new_texture_filename) const;

	void RenderPose(const gef::SkeletonPose& pose, float size);
};

#endif // _ASSIMP_VIEWER_H
