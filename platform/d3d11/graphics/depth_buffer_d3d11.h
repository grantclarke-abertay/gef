#ifndef _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H
#define _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H

#include <graphics/depth_buffer.h>
#include <d3d11.h>

namespace gef
{
	class DepthBufferD3D11 : public DepthBuffer
	{
	public:
		DepthBufferD3D11(ID3D11Device* device, UInt32 width, UInt32 height);
		~DepthBufferD3D11();

		void Release();

		inline ID3D11DepthStencilView* depth_stencil_view() const { return depth_stencil_view_; }

	protected:
		ID3D11Device* device_;
		ID3D11Texture2D* depth_stencil_;
		ID3D11DepthStencilView* depth_stencil_view_;
	};
}

#endif // _GEF_GRAPHICS_D3D11_DEPTH_BUFFER_D3D11_H
