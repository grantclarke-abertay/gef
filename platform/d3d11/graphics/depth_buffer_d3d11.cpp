#include <platform/d3d11/graphics/depth_buffer_d3d11.h>

namespace gef
{
	DepthBufferD3D11::DepthBufferD3D11(ID3D11Device* device, UInt32 width, UInt32 height) :
		DepthBuffer(width, height),
		device_(device),
		depth_stencil_(NULL),
		depth_stencil_view_(NULL)
	{
		HRESULT hresult = S_OK;
		D3D11_TEXTURE2D_DESC desc_depth;
		desc_depth.Width = width_;
		desc_depth.Height = height_;
		desc_depth.MipLevels = 1;
		desc_depth.ArraySize = 1;
		desc_depth.Format = DXGI_FORMAT_D32_FLOAT;
		desc_depth.SampleDesc.Count = 1;
		desc_depth.SampleDesc.Quality = 0;
		desc_depth.Usage = D3D11_USAGE_DEFAULT;
		desc_depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc_depth.CPUAccessFlags = 0;
		desc_depth.MiscFlags = 0;
		hresult = device_->CreateTexture2D(&desc_depth, NULL, &depth_stencil_);

		if (SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc_DSV;
			ZeroMemory(&desc_DSV, sizeof(desc_DSV));
			desc_DSV.Format = desc_depth.Format;
			desc_DSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc_DSV.Texture2D.MipSlice = 0;
			hresult = device_->CreateDepthStencilView(depth_stencil_, &desc_DSV, &this->depth_stencil_view_);
		}
	}

	DepthBufferD3D11::~DepthBufferD3D11()
	{
		Release();
	}

	void DepthBufferD3D11::Release()
	{
		ReleaseNull(depth_stencil_view_);
		ReleaseNull(depth_stencil_);
	}
}