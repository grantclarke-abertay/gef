#include <platform/d3d11/graphics/depth_buffer_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/d3d11/graphics/texture_d3d11.h>

namespace gef
{
	DepthBuffer* DepthBuffer::Create(const Platform& platform, UInt32 width, UInt32 height)
	{
		return new DepthBufferD3D11(
			static_cast<const PlatformD3D11&>(platform).device(),
			static_cast<const PlatformD3D11&>(platform).device_context(), width, height);
	}

	DepthBufferD3D11::DepthBufferD3D11(ID3D11Device* device, ID3D11DeviceContext* device_context, UInt32 width, UInt32 height, bool as_shader_resource) :
		DepthBuffer(width, height),
		device_(device),
		depth_stencil_(NULL),
		depth_stencil_view_(NULL),
		depth_stencil_shader_resource_(NULL)
	{
		HRESULT hresult = S_OK;
		D3D11_TEXTURE2D_DESC desc_depth;
		desc_depth.Width = width_;
		desc_depth.Height = height_;
		desc_depth.MipLevels = 1;
		desc_depth.ArraySize = 1;

		if(as_shader_resource)
			desc_depth.Format = DXGI_FORMAT_R32_TYPELESS;
		else
			desc_depth.Format = DXGI_FORMAT_D32_FLOAT;
		desc_depth.SampleDesc.Count = 1;
		desc_depth.SampleDesc.Quality = 0;
		desc_depth.Usage = D3D11_USAGE_DEFAULT;
		desc_depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		if (as_shader_resource)
			desc_depth.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

		desc_depth.CPUAccessFlags = 0;
		desc_depth.MiscFlags = 0;
		hresult = device_->CreateTexture2D(&desc_depth, NULL, &depth_stencil_);

		if (SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc_DSV;
			ZeroMemory(&desc_DSV, sizeof(desc_DSV));
			desc_DSV.Format = DXGI_FORMAT_D32_FLOAT;
			desc_DSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc_DSV.Texture2D.MipSlice = 0;
			hresult = device_->CreateDepthStencilView(depth_stencil_, &desc_DSV, &this->depth_stencil_view_);
		}

		if (SUCCEEDED(hresult) && as_shader_resource)
		{
			//create shader resource view desc
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = desc_depth.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;

			hresult = device_->CreateShaderResourceView(depth_stencil_, &srvDesc, &this->depth_stencil_shader_resource_);

			if (SUCCEEDED(hresult))
			{
				TextureD3D11* texture_d3d11 = new TextureD3D11(device_context);
				texture_ = texture_d3d11;
				// texture now owns shader resource view
				texture_d3d11->set_shader_resource_view(depth_stencil_shader_resource_);
			}
		}
	}

	DepthBufferD3D11::~DepthBufferD3D11()
	{
		Release();
	}

	void DepthBufferD3D11::Release()
	{
		// texture owns shader resource view
		//ReleaseNull(depth_stencil_shader_resource_);
		ReleaseNull(depth_stencil_view_);
		ReleaseNull(depth_stencil_);
	}
}