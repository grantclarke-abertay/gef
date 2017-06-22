#ifndef _GEF_SPRITE_RENDERER_D3D11_H
#define _GEF_SPRITE_RENDERER_D3D11_H

#include <graphics/sprite_renderer.h>
#include <d3d11.h>

namespace gef
{
	class Platform;
	class Texture;
	class VertexBuffer;

	class SpriteRendererD3D11 : public SpriteRenderer
	{
	public:
		SpriteRendererD3D11(Platform& platform);
		~SpriteRendererD3D11();

		void Begin(bool clear = true);
		void DrawSprite(const Sprite& sprite);
		void End();

	private:
		void CleanUp();

		Texture* default_texture_;
		VertexBuffer* vertex_buffer_;

		ID3D11RasterizerState* default_render_state_;
		ID3D11BlendState* default_blend_state_;
		ID3D11DepthStencilState* default_depth_stencil_state_;

	};
}
#if 0
#include <maths/vector4.h>
#include <maths/matrix44.h>
#include <d3d11.h>

namespace gef
{
	class Sprite;
	class DefaultSpriteShaderD3D11;
	class TextureD3D11;
	class Platform;

	struct SpriteVertex
	{
		Vector4 position;
	};

	class SpriteRendererD3D11 : public SpriteRenderer
	{
	public:
		SpriteRendererD3D11(Platform& platform);
		~SpriteRendererD3D11();
		void Begin(bool clear);
		void End();
		void DrawSprite(const Sprite& sprite);
	private:
		void CleanUp();

		ID3D11RasterizerState* default_render_state_;
		ID3D11BlendState* default_blend_state_;
		ID3D11DepthStencilState* default_depth_stencil_state_;
		ID3D11Buffer* vertex_buffer_;
		ID3D11Buffer*      index_buffer_;
		TextureD3D11* default_texture_;
	};

}
#endif
#endif // _GEF_SPRITE_RENDERER_D3D11_H
