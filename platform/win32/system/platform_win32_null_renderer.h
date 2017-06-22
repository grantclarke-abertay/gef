#ifndef _GEF_PLATFORM_WIN32_NULL_RENDER_H
#define _GEF_PLATFORM_WIN32_NULL_RENDER_H

#include <system/platform.h>

namespace gef
{
	class PlatformWin32NullRenderer : public Platform
	{
	public:
		PlatformWin32NullRenderer();
		~PlatformWin32NullRenderer();

		bool Update();
		float GetFrameTime();
		void PreRender();
		void PostRender();
		void Clear() const;

		std::string FormatFilename(const std::string& filename) const;
		std::string FormatFilename(const char* filename) const;

		class SpriteRenderer* CreateSpriteRenderer();
		class Renderer3D* CreateRenderer3D();
		class InputManager* CreateInputManager();

#if 0
		class SpriteRenderer* CreateSpriteRenderer();
		File* CreateFile() const;
		AudioManager* CreateAudioManager() const;
		class Texture* CreateTexture(const ImageData& image_data) const;
		Mesh* CreateMesh();
		InputManager* CreateInputManager();
		class Renderer3D* CreateRenderer3D();
		class VertexBuffer* CreateVertexBuffer() const;
		class IndexBuffer* CreateIndexBuffer() const;
		class ShaderInterface* CreateShaderInterface() const;
		virtual DepthBuffer* CreateDepthBuffer(UInt32 width, UInt32 height) const;
		RenderTarget* CreateRenderTarget(const Int32 width, const Int32 height) const;
#endif

		Matrix44 PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance, const float far_distance) const;
		Matrix44 PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const;
		Matrix44 OrthographicFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const;

		void Clear(const bool clear_render_target, const bool clear_depth_buffer, const bool clear_stencil_buffer) const;


		void BeginScene() const;
		void EndScene() const;
		const char* GetShaderDirectory() const;
		const char* GetShaderFileExtension() const;




	private:


	};

}

#endif // _GEF_PLATFORM_WIN32_NULL_RENDER_H
