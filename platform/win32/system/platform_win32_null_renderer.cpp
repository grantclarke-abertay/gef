#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <platform/win32/system/platform_win32_null_renderer.h>
#include <platform/win32/system/file_win32.h>
#include <maths/matrix44.h>

namespace gef
{
	PlatformWin32NullRenderer::PlatformWin32NullRenderer()
	{
	}

	PlatformWin32NullRenderer::~PlatformWin32NullRenderer()
	{
	}



	void PlatformWin32NullRenderer::PreRender()
	{
	}

	void PlatformWin32NullRenderer::PostRender()
	{
	}

	void PlatformWin32NullRenderer::Clear() const
	{
	}

	bool PlatformWin32NullRenderer::Update()
	{
		return true;
	}

	float PlatformWin32NullRenderer::GetFrameTime()
	{
		return 0.0f;
	}

	std::string PlatformWin32NullRenderer::FormatFilename(const std::string& filename) const
	{
		return filename;
	}

	std::string PlatformWin32NullRenderer::FormatFilename(const char* filename) const
	{
		return std::string(filename);
	}


	Matrix44 PlatformWin32NullRenderer::PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.SetIdentity();
		return projection_matrix;
	}

	Matrix44 PlatformWin32NullRenderer::PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.SetIdentity();
		return projection_matrix;
	}

	Matrix44 PlatformWin32NullRenderer::OrthographicFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.SetIdentity();
		return projection_matrix;
	}

	void PlatformWin32NullRenderer::BeginScene() const
	{
	}

	void PlatformWin32NullRenderer::EndScene() const
	{
	}


	const char* PlatformWin32NullRenderer::GetShaderDirectory() const
	{
		return NULL;
	}
	const char* PlatformWin32NullRenderer::GetShaderFileExtension() const
	{
		return NULL;
	}

	class SpriteRenderer* PlatformWin32NullRenderer::CreateSpriteRenderer()
	{
		return NULL;
	}

	Renderer3D* PlatformWin32NullRenderer::CreateRenderer3D()
	{
		return NULL;
	}

	class InputManager* PlatformWin32NullRenderer::CreateInputManager()
	{
		return NULL;
	}

#if 0

	class Mesh* PlatformWin32NullRenderer::CreateMesh()
	{
		return NULL;
	}

	class Texture* PlatformWin32NullRenderer::CreateTexture(const ImageData& image_data) const
	{
		return NULL;
	}

	File* PlatformWin32NullRenderer::CreateFile() const
	{
		return new gef::FileWin32();
	}

	AudioManager* PlatformWin32NullRenderer::CreateAudioManager() const
	{
		return NULL;
	}

	//	TouchInputManager* PlatformWin32NullRenderer::CreateTouchInputManager() const
	//	{
	//		return NULL;
	//	}



	RenderTarget* PlatformWin32NullRenderer::CreateRenderTarget(const Int32 width, const Int32 height) const
	{
		return NULL;
	}

	class VertexBuffer* PlatformWin32NullRenderer::CreateVertexBuffer() const
	{
		return NULL;
	}

	class IndexBuffer* PlatformWin32NullRenderer::CreateIndexBuffer() const
	{
		return NULL;
	}

	class ShaderInterface* PlatformWin32NullRenderer::CreateShaderInterface() const
	{
		return NULL;
	}

	DepthBuffer* PlatformWin32NullRenderer::CreateDepthBuffer(UInt32 width, UInt32 height) const
	{
		return NULL;
	}
#endif

}