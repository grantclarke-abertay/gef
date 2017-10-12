#include <platform/d3d11/graphics/render_target_d3d11.h>
#include <gef.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/d3d11/graphics/texture_d3d11.h>

namespace gef
{
	RenderTarget* RenderTarget::Create(const Platform& platform, const Int32 width, const Int32 height)
	{
		return new RenderTargetD3D11(platform, width, height);
	}

	RenderTargetD3D11::RenderTargetD3D11(const Platform& platform, const Int32 width, const Int32 height) :
		RenderTarget(platform, width, height),
		render_target_view_(NULL)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);

		TextureD3D11* texture_d3d11 = new TextureD3D11(platform_d3d.device_context());
		texture_ = texture_d3d11;

		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT result;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// Initialize the render target texture description.
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Setup the render target texture description.
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Create the render target texture.
		ID3D11Texture2D* render_target_texture = NULL;
		ID3D11ShaderResourceView* shader_resource_view = NULL;
		result = platform_d3d.device()->CreateTexture2D(&textureDesc, NULL, &render_target_texture);
		if(SUCCEEDED(result))
		{
			texture_d3d11->set_texture(render_target_texture); 

			// Setup the description of the render target view.
			renderTargetViewDesc.Format = textureDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			// Create the render target view.
			result = platform_d3d.device()->CreateRenderTargetView(render_target_texture, &renderTargetViewDesc, &render_target_view_);
		}

		
		if(SUCCEEDED(result))
		{
			// Setup the description of the shader resource view.
			shaderResourceViewDesc.Format = textureDesc.Format;
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;

			// Create the shader resource view.
			result = platform_d3d.device()->CreateShaderResourceView(render_target_texture, &shaderResourceViewDesc, &shader_resource_view);
		}

		if(FAILED(result))
		{
			CleanUp();
		}
		else
		{
			// texture now owns shader resource view
			texture_d3d11->set_shader_resource_view(shader_resource_view); 
		}
	}

	RenderTargetD3D11::~RenderTargetD3D11()
	{
		CleanUp();
	}

	void RenderTargetD3D11::CleanUp()
	{
		DeleteNull(texture_);
		ReleaseNull(render_target_view_);
	}

	void RenderTargetD3D11::Begin(const Platform& platform)
	{
	}

	void RenderTargetD3D11::End(const Platform& platform)
	{
	}
}