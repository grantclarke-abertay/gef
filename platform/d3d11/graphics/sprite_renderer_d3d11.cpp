#include <platform/d3d11/graphics/sprite_renderer_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>

#include <system/platform.h>
#include <graphics/texture.h>
#include <graphics/vertex_buffer.h>
#include <graphics/sprite.h>
#include <graphics/shader_interface.h>

namespace gef
{
	SpriteRenderer* SpriteRenderer::Create(Platform& platform)
	{
		return new SpriteRendererD3D11(platform);
	}

	SpriteRendererD3D11::SpriteRendererD3D11(Platform& platform)
		:SpriteRenderer(platform)
		,vertex_buffer_(NULL)
		,default_render_state_(NULL)
		,default_blend_state_(NULL)
		,default_depth_stencil_state_(NULL)

	{
		vertex_buffer_ = gef::VertexBuffer::Create(platform);

		float vertices[] = {-0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f,    // triangle 1
			-0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f, -0.5f,0.5f,0.0f};   // triangle 2
		vertex_buffer_->Init(platform, vertices, 6, sizeof(float)*3);
		platform_.AddVertexBuffer(vertex_buffer_);

		default_texture_ = Texture::CreateCheckerTexture(16, 1, platform);
		platform_.AddTexture(default_texture_);

		platform_.AddShader(&default_shader_);
		shader_ = &default_shader_;

		projection_matrix_ = platform_.OrthographicFrustum(0.0f, (float)platform_.width(), 0.0f, (float)platform_.height(), -1.0f, 1.0f);

		HRESULT hresult = S_OK;
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);

		D3D11_RASTERIZER_DESC render_state_desc;
		ZeroMemory(&render_state_desc, sizeof(D3D11_RASTERIZER_DESC));
		render_state_desc.FillMode = D3D11_FILL_SOLID;
		render_state_desc.CullMode = D3D11_CULL_BACK;
		//	render_state_desc.CullMode = D3D11_CULL_NONE;
		hresult = platform_d3d.device()->CreateRasterizerState(&render_state_desc, &default_render_state_);


		if (SUCCEEDED(hresult))
		{

			D3D11_RENDER_TARGET_BLEND_DESC rtbd;
			ZeroMemory(&rtbd, sizeof(rtbd));
			rtbd.BlendEnable = true;
			rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.BlendOp = D3D11_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbd.RenderTargetWriteMask = 0x0f;

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));
			blendDesc.RenderTarget[0] = rtbd;

			hresult = platform_d3d.device()->CreateBlendState(&blendDesc, &default_blend_state_);
		}

		if (SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc;
			// Clear the second depth stencil state before setting the parameters.
			ZeroMemory(&depth_stencil_state_desc, sizeof(depth_stencil_state_desc));

			// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
			// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
			depth_stencil_state_desc.DepthEnable = true;
			depth_stencil_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depth_stencil_state_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			depth_stencil_state_desc.StencilEnable = false;

			// Create the state using the device.
			hresult = platform_d3d.device()->CreateDepthStencilState(&depth_stencil_state_desc, &default_depth_stencil_state_);

		}

		if (FAILED(hresult))
		{
			CleanUp();
		}
	}

	SpriteRendererD3D11::~SpriteRendererD3D11()
	{
		CleanUp();
	}

	void SpriteRendererD3D11::CleanUp()
	{
		ReleaseNull(default_blend_state_);
		ReleaseNull(default_render_state_);
		ReleaseNull(default_depth_stencil_state_);

		platform_.RemoveShader(&default_shader_);

		if (vertex_buffer_)
		{
			platform_.RemoveVertexBuffer(vertex_buffer_);
			delete vertex_buffer_;
		}

		if (default_texture_)
		{
			platform_.RemoveTexture(default_texture_);
			delete default_texture_;
		}
	}


	void SpriteRendererD3D11::Begin(bool clear)
	{
		platform_.BeginScene();
		if(clear)
			platform_.Clear();

		vertex_buffer_->Bind(platform_);

		if (shader_ == &default_shader_)
		{

			default_shader_.device_interface()->UseProgram();


			default_shader_.SetSceneData(projection_matrix_);

			default_shader_.device_interface()->SetVertexFormat();
		}

		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform_);
		platform_d3d.device_context()->RSSetState(default_render_state_);
		platform_d3d.device_context()->OMSetBlendState(default_blend_state_, NULL, 0xffffffff);
		platform_d3d.device_context()->OMSetDepthStencilState(default_depth_stencil_state_, 0);

		platform_d3d.device_context()->GSSetShader(nullptr, nullptr, 0u);
	}

	void SpriteRendererD3D11::DrawSprite(const Sprite& sprite)
	{
		if (shader_ == &default_shader_)
		{
			const Texture* texture = sprite.texture();
			if (!texture)
				texture = default_texture_;

			default_shader_.SetSpriteData(sprite, texture);
			default_shader_.device_interface()->SetVariableData();

			default_shader_.device_interface()->BindTextureResources(platform_);
		}

		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform_);


		platform_d3d.device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		platform_d3d.device_context()->Draw(6, 0);

		if (shader_ == &default_shader_)
		{
			default_shader_.device_interface()->UnbindTextureResources(platform_);
		}
	}

	void SpriteRendererD3D11::End()
	{
		vertex_buffer_->Unbind(platform_);

		platform_.EndScene();
	}
}

