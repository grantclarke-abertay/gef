#ifndef _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H
#define _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H

#include <graphics/depth_buffer.h>
#include <d3d11.h>

namespace gef
{
	class TextureD3D11;

	class DepthBufferD3D11 : public DepthBuffer
	{
	public:
		DepthBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* device_context, UInt32 width, UInt32 height, bool as_shader_resource = false);
		~DepthBufferD3D11();

		void Release();

		inline ID3D11DepthStencilView* depth_stencil_view() const { return depth_stencil_view_; }
	protected:
		ID3D11Device* device_;
		ID3D11Texture2D* depth_stencil_;
		ID3D11DepthStencilView* depth_stencil_view_;
		ID3D11ShaderResourceView* depth_stencil_shader_resource_;
	};
}

#endif // _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H
