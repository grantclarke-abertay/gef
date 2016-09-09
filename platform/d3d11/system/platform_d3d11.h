#ifndef _GEF_PLATFORM_D3D11_H
#define _GEF_PLATFORM_D3D11_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>

#include <system/platform.h>
//#include "colour.h"

namespace gef
{
	class WindowWin32;
	class Mesh;
	class VertexBuffer;
	class IndexBuffer;
    class DepthBuffer;

	class PlatformD3D11 : public Platform
	{
	public:
		PlatformD3D11(HINSTANCE hinstance, UInt32 width, UInt32 height, bool fullscreen, bool vsync_enabled, HWND hwnd = NULL);
		~PlatformD3D11();
		void Resize(UInt32 width, UInt32 height);
		class Renderer3D* CreateRenderer3D();
		class SpriteRenderer* CreateSpriteRenderer();
		Mesh* CreateMesh();
		class Texture* CreateTexture(const ImageData& image_data) const;
		InputManager* CreateInputManager();
		VertexBuffer* CreateVertexBuffer() const;
		IndexBuffer* CreateIndexBuffer() const;
//		class Texture* CreateCheckerTexture(UInt32 size, UInt32 num_checkers) ;
		Matrix44 PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance, const float far_distance) const;
		Matrix44 PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const;
		Matrix44 OrthographicFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const;
//		void OrthographicFrustumLH(Matrix44& projection_matrix, const float left, const float right, const float top, const float bottom, const float near, const float far);
		void Clear(const bool clear_render_target, const bool clear_depth_buffer, const bool clear_stencil_buffer) const;


		void BeginScene() const;
		void EndScene() const;


		void PreRender();
		void PostRender();
		void Clear() const;

		bool Update();
		float GetFrameTime();
		std::string FormatFilename(const std::string& filename) const;
		std::string FormatFilename(const char* filename) const;
		File* CreateFile() const;
		AudioManager* CreateAudioManager() const;
		//void InitTouchInputManager();
		//void ReleaseTouchInputManager();

		ShaderInterface* PlatformD3D11::CreateShaderInterface() const;
		const char* GetShaderDirectory() const;
		const char* GetShaderFileExtension() const;

		DepthBuffer* CreateDepthBuffer(UInt32 width, UInt32 height) const;


		inline ID3D11Device* device() const { return device_; } 
		inline ID3D11DeviceContext* device_context() const { return device_context_; }
		inline WindowWin32* window() const { return window_; }
		inline HWND hwnd() const { return hwnd_; }
		inline void set_top_level_hwnd(HWND parent_hwnd) {top_level_hwnd_ = parent_hwnd;}
		inline HWND top_level_hwnd() const { return top_level_hwnd_; }
	private:
		RenderTarget* CreateRenderTarget(const Int32 width, const Int32 height) const;
		ID3D11RenderTargetView* GetRenderTargetView() const;
		ID3D11DepthStencilView* GetDepthStencilView() const;

		void Release();
		bool InitDeviceAndSwapChain(bool fullscreen);
		void ReleaseDeviceAndSwapChain();
		HRESULT CreateDepthStencilBuffer();
		void SetupViewport() const;

		WindowWin32* window_;
		UInt64		clock_last_frame_;
		D3D_DRIVER_TYPE driver_type_;
		ID3D11Device* device_;
		ID3D11DeviceContext* device_context_;
		IDXGISwapChain* swap_chain_;
		ID3D11Texture2D* back_buffer_;
		ID3D11RenderTargetView* render_target_view_;
		ID3D11Texture2D* depth_stencil_;
		ID3D11DepthStencilView* depth_stencil_view_;
		bool vsync_enabled_;
		HWND hwnd_;
		HWND top_level_hwnd_;
	};
}
#endif // _GEF_PLATFORM_D3D11_H
