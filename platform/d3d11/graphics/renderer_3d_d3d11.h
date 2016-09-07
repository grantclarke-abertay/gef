#ifndef _GEF_RENDERER_3D_D3D_H
#define _GEF_RENDERER_3D_D3D_H

#include <graphics/renderer_3d.h>
#include <graphics/primitive.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>


#include <graphics/renderer_3d.h>
//#include <graphics/primitive.h>
#include <graphics/default_3d_shader.h>

namespace gef
{
	// FORWARD DECLARATIONS
	class Texture;

	class Renderer3DD3D11 : public Renderer3D
	{
	public:
		Renderer3DD3D11(Platform& platform);
		~Renderer3DD3D11();

		void CleanUp();

		void Begin(bool clear = true);
		void End();

		void DrawMesh(const  MeshInstance& mesh_instance);
		void DrawPrimitive(const  MeshInstance& mesh_instance, Int32 primitive_index, Int32 num_indices = -1);
		void SetFillMode(FillMode fill_mode);
		void SetDepthTest(DepthTest depth_test);

	protected:
		static const D3D11_PRIMITIVE_TOPOLOGY Renderer3DD3D11::primitive_types[NUM_PRIMITIVE_TYPES];

		Default3DShader default_shader_;

	private:
		ID3D11RasterizerState* default_render_state_;
		ID3D11RasterizerState* wireframe_render_state_;
		ID3D11BlendState* default_blend_state_;

		ID3D11DepthStencilState* default_depth_stencil_state_;
		ID3D11DepthStencilState* always_depth_stencil_state_;
	};
}


/*
namespace gef
{
	class Platform;
	class MeshInstance;
	class ShaderD3D11;
	class TextureD3D11;
	class Material;

	class Renderer3DD3D11 : public Renderer3D
	{
	public:
		Renderer3DD3D11(Platform& platform);
		~Renderer3DD3D11();
		void CleanUp();

		void Begin(bool clear);
		void End();
		void DrawMesh(const class MeshInstance& mesh_instance);
		void SetFillMode(FillMode fill_mode);
		void SetDepthTest(DepthTest depth_test);
		void DrawPrimitive(const  MeshInstance& mesh_instance, Int32 primitive_index, Int32 num_indices);


		void ClearZBuffer();

		inline const Material* current_material() const {return current_material_; }
	private:
		ID3D11RasterizerState* default_render_state_;
		ID3D11RasterizerState* wireframe_render_state_;
		ID3D11BlendState* default_blend_state_;
		TextureD3D11* default_texture_;

		ID3D11DepthStencilState* default_depth_stencil_state_;
		ID3D11DepthStencilState* always_depth_stencil_state_;

		const Material* current_material_;

		static const D3D11_PRIMITIVE_TOPOLOGY Renderer3DD3D11::primitive_types[NUM_PRIMITIVE_TYPES];
	};
}
*/
#endif // _GEF_RENDERER_3D_D3D_H
