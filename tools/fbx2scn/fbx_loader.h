#ifndef _ABFW_FBX_LOADER_H
#define _ABFW_FBX_LOADER_H

namespace gef
{
	class Scene;
	class Platform;

	class FBXLoader
	{
	public:
		FBXLoader();
		~FBXLoader();

		bool IsFileFormatSupported();
		bool Load(const char* const filename, Platform& platform, Scene& scene, const bool anim_data_only = false);

		void set_ignore_skinning(const bool ignore_skinning) { ignore_skinning_ = ignore_skinning; }
		void set_scaling_factor(const float scaling_factor) { scaling_factor_ = scaling_factor; }
		void set_strip_texture_path(const bool strip_texture_path) { strip_texture_path_ = strip_texture_path; }
		void set_texture_filename_ext(const char* texture_filename_ext) { texture_filename_ext_ = texture_filename_ext; }

	private:
		float scaling_factor_;
		const char* texture_filename_ext_;
		bool strip_texture_path_;
		bool ignore_skinning_;
	};
}

#endif // _FBX_LOADER_H