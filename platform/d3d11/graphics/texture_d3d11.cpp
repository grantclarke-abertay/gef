#include <platform/d3d11/graphics/texture_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <graphics/image_data.h>
#include <gef.h>

namespace gef
{
	Texture* Texture::Create(const Platform& platform, const ImageData& image_data)
	{
		return new TextureD3D11(platform, image_data);
	}

	TextureD3D11::TextureD3D11(ID3D11DeviceContext* device_context) :
	texture_(NULL),
	shader_resource_view_(NULL),
	image_data_buffer_(NULL),
	device_context_(device_context)
{
}


TextureD3D11::TextureD3D11(const Platform& platform, const ImageData& image_data) :
	texture_(NULL),
	shader_resource_view_(NULL),
	image_data_buffer_(NULL),
	device_context_(NULL),
	Texture(platform, image_data)
{
	// get the size of the texture data
	const UInt32 data_size = image_data.width()*image_data.height()*4;
	image_data_buffer_ = new UInt8[data_size];
	memcpy(image_data_buffer_, image_data.image(),data_size);

	CreateTexture(platform, image_data.width(), image_data.height(), (UInt8*)image_data_buffer_, true);

	device_context_ = static_cast<const PlatformD3D11&>(platform).device_context();
}

TextureD3D11::~TextureD3D11()
{
	Release();
}

void TextureD3D11::Release()
{
	ReleaseNull(shader_resource_view_);
	ReleaseNull(texture_);
	DeleteArrayNull(image_data_buffer_);
	device_context_ = NULL;
}

void TextureD3D11::Bind(ID3D11DeviceContext* device_context, Int32 slot) const
{
	// Set shader texture resource in the pixel shader.
	device_context->PSSetShaderResources(slot, 1, &shader_resource_view_);
}

void TextureD3D11::Unbind(ID3D11DeviceContext* device_context, Int32 slot) const
{
	// Set shader texture resource in the pixel shader.
	ID3D11ShaderResourceView* shader_resource_view = NULL;
	device_context->PSSetShaderResources(slot, 1, &shader_resource_view);
}

void TextureD3D11::Bind(const Platform& platform, const int texture_stage_num) const
{
	Bind(device_context_, texture_stage_num);
}

void TextureD3D11::Unbind(const Platform& platform, const int texture_stage_num) const
{
	Unbind(device_context_, texture_stage_num);
}

bool TextureD3D11::CreateTexture(const class Platform& platform, Int32 width, Int32 height, UInt8* image_data, bool has_alpha)
{
	const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

	D3D11_SUBRESOURCE_DATA initial_data;
	initial_data.pSysMem = image_data;
	initial_data.SysMemPitch = width*4; // 4 bytes - r g b a
	initial_data.SysMemSlicePitch = height*width*4; // only used for 3D textures

	D3D11_TEXTURE2D_DESC texture_desc;
	texture_desc.Width = width;
	texture_desc.Height = height;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = format;
	texture_desc.SampleDesc.Count = 1;
	texture_desc.SampleDesc.Quality = 0;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;
	HRESULT hresult = S_OK;
	hresult = platform_d3d.device()->CreateTexture2D( &texture_desc, &initial_data, &texture_ );



	if(SUCCEEDED(hresult))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc_SRV;
		ZeroMemory(&desc_SRV, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc_SRV.Format = format;
		desc_SRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc_SRV.Texture2D.MipLevels = 1;
		hresult = platform_d3d.device()->CreateShaderResourceView(texture_, &desc_SRV, &shader_resource_view_);
	}

	if(SUCCEEDED(hresult))
	{
		return true;
	}
	else
	{
		Release();
		return false;
	}
}

bool TextureD3D11::CreateTexture(const class Platform& platform, const D3D11_TEXTURE2D_DESC& texture_desc, const D3D11_SUBRESOURCE_DATA* const subresource_data)
{
	const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);
	HRESULT hresult = S_OK;
	hresult = platform_d3d.device()->CreateTexture2D( &texture_desc, subresource_data, &texture_ );

	if(SUCCEEDED(hresult))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc_SRV;
		ZeroMemory(&desc_SRV, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc_SRV.Format = texture_desc.Format;
		desc_SRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc_SRV.Texture2D.MipLevels = 1;
		hresult = platform_d3d.device()->CreateShaderResourceView(texture_, &desc_SRV, &shader_resource_view_);
	}

	if(SUCCEEDED(hresult))
	{
		return true;
	}
	else
	{
		Release();
		return false;
	}
}

}