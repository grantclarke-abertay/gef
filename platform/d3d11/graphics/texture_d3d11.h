#ifndef _GEF_TEXTURE_D3D_H
#define _GEF_TEXTURE_D3D_H

#include <graphics/texture.h>
#include <d3d11.h>

namespace gef
{

class TextureD3D11 : public Texture
{
public:
	TextureD3D11(ID3D11DeviceContext* device_context);
	TextureD3D11(const class Platform& platform, const ImageData& image_data);
	~TextureD3D11();
	void Bind(ID3D11DeviceContext* device_context, Int32 slot = 0) const;
	void Unbind(ID3D11DeviceContext* device_context, Int32 slot = 0) const;

	void Bind(const Platform& platform, const int texture_stage_num) const;
	void Unbind(const Platform& platform, const int texture_stage_num) const;

	inline ID3D11ShaderResourceView* shader_resource_view() const { return shader_resource_view_; }
	inline void set_shader_resource_view(ID3D11ShaderResourceView* shader_resource_view) {shader_resource_view_ = shader_resource_view; }
	inline ID3D11Texture2D* texture() const {return texture_; }
	inline  void set_texture(ID3D11Texture2D* texture) {texture_ = texture; }

private:
	bool CreateTexture(const class Platform& platform, Int32 width, Int32 height, UInt8* image_data, bool has_alpha);
	bool CreateTexture(const class Platform& platform, const D3D11_TEXTURE2D_DESC& texture_desc, const D3D11_SUBRESOURCE_DATA* const subresource_data);
	void Release();

	ID3D11DeviceContext* device_context_;
	ID3D11Texture2D* texture_;
	ID3D11ShaderResourceView* shader_resource_view_;
	void* image_data_buffer_;
};

}
#endif // _GEF_TEXTURE_D3D_H
