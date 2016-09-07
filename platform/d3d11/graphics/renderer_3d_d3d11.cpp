#include <platform/d3d11/graphics/renderer_3d_d3d11.h>

/*
* renderer_3d_gl.cpp
*
*  Created on: 21 Jan 2015
*      Author: grant
*/

#include <platform/d3d11/system/platform_d3d11.h>
#include <graphics/mesh_instance.h>
//#include <graphics/gl/shader_gl.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <graphics/vertex_buffer.h>
//#include <graphics/gl/index_buffer_gl.h>
//#include <graphics/gl/default_3d_shader_gl.h>
//#include <graphics/gl/texture_gl.h>
#include <graphics/material.h>
#include <graphics/texture.h>
#include <graphics/index_buffer.h>
#include <graphics/shader_interface.h>

namespace gef
{
	/*
	const GLenum Renderer3DD3D11::primitive_types[NUM_PRIMITIVE_TYPES] =
	{
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_LINES,

	};
*/
	const D3D11_PRIMITIVE_TOPOLOGY Renderer3DD3D11::primitive_types[NUM_PRIMITIVE_TYPES] =
	{
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,

	};


	Renderer3DD3D11::Renderer3DD3D11(Platform& platform) :
		Renderer3D(platform)
		,default_shader_(platform)
		,default_render_state_(NULL)
		,default_blend_state_(NULL)
		,default_depth_stencil_state_(NULL)
		,always_depth_stencil_state_(NULL)

	{
		platform_.AddShader(&default_shader_);
		shader_ = &default_shader_;

		projection_matrix_.SetIdentity();

		HRESULT hresult = S_OK;
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform_);

		D3D11_RASTERIZER_DESC render_state_desc;
		ZeroMemory(&render_state_desc, sizeof(D3D11_RASTERIZER_DESC));
		render_state_desc.FillMode = D3D11_FILL_SOLID;
		render_state_desc.CullMode = D3D11_CULL_BACK;

		hresult = platform_d3d.device()->CreateRasterizerState(&render_state_desc, &default_render_state_);

		if (SUCCEEDED(hresult))
		{
			render_state_desc.FillMode = D3D11_FILL_WIREFRAME;
			hresult = platform_d3d.device()->CreateRasterizerState(&render_state_desc, &wireframe_render_state_);
		}

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
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			// Stencil test parameters
			dsDesc.StencilEnable = false;

			// Create depth stencil state
			platform_d3d.device()->CreateDepthStencilState(&dsDesc, &default_depth_stencil_state_);
		}

		if (SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil test parameters
			dsDesc.StencilEnable = false;

			// Create depth stencil state
			platform_d3d.device()->CreateDepthStencilState(&dsDesc, &always_depth_stencil_state_);
		}


		if (FAILED(hresult))
		{
			CleanUp();
		}
	}

	Renderer3DD3D11::~Renderer3DD3D11()
	{
		CleanUp();
	}

	void Renderer3DD3D11::CleanUp()
	{
		ReleaseNull(default_render_state_);
		ReleaseNull(wireframe_render_state_);
		ReleaseNull(default_blend_state_);
		ReleaseNull(default_depth_stencil_state_);
		ReleaseNull(always_depth_stencil_state_);

		platform_.RemoveShader(&default_shader_);

	}

	void Renderer3DD3D11::Begin(bool clear)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
		platform_d3d.BeginScene();

		if (clear)
			platform_d3d.Clear();

		platform_d3d.device_context()->RSSetState(default_render_state_);
		platform_d3d.device_context()->OMSetBlendState(default_blend_state_, NULL, 0xffffffff);
		platform_d3d.device_context()->OMSetDepthStencilState(default_depth_stencil_state_, 0);
	}

	void Renderer3DD3D11::End()
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
		platform_d3d.EndScene();
	}

	void Renderer3DD3D11::DrawMesh(const  MeshInstance& mesh_instance)
	{
		// set up the shader data for default shader
		if (shader_ == &default_shader_)
			default_shader_.SetSceneData(default_shader_data_, view_matrix_, projection_matrix_);

		const Mesh* mesh = mesh_instance.mesh();
		if(mesh != NULL)
		{
			set_world_matrix(mesh_instance.transform());

			const VertexBuffer* vertex_buffer = mesh->vertex_buffer();
			//ShaderGL* shader_GL = static_cast<ShaderGL*>(shader_);

			if(vertex_buffer && shader_)
			{
				shader_->SetMeshData(mesh_instance);

				shader_->device_interface()->UseProgram();
				vertex_buffer->Bind(platform_);

				// vertex format must be set after the vertex buffer is bound
				shader_->device_interface()->SetVertexFormat();

				for(UInt32 primitive_index=0;primitive_index<mesh->num_primitives();++primitive_index)
				{
					const Primitive* primitive = mesh->GetPrimitive(primitive_index);
					const IndexBuffer* index_buffer = primitive->index_buffer();
					if(primitive->type() != UNDEFINED && index_buffer)
					{
						// default texture
						const Material* material;
						if (override_material_)
							material = override_material_;
						else
							material = primitive->material();


						//only set default shader data if current shader is the default shader
						shader_->SetMaterialData(material);

						// GRC FIXME - probably want to split variable data into scene, object, primitive[material?] based
						// rather than set all variables per primitive
						shader_->device_interface()->SetVariableData();
						shader_->device_interface()->BindTextureResources(platform());

						//GLenum primitive_type = primitive_types[primitive->type()];

						// Set primitive topology
						const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform_);
						platform_d3d.device_context()->IASetPrimitiveTopology(primitive_types[primitive->type()]);

						index_buffer->Bind(platform_);

						// use the primitive end index to specify how may indices we wish to draw
						// in case we don't want to draw them all

						if (index_buffer->num_indices() > 0)
							platform_d3d.device_context()->DrawIndexed(index_buffer->num_indices(), 0, 0);
						else
							platform_d3d.device_context()->Draw(vertex_buffer->num_vertices(), 0);


						index_buffer->Unbind(platform_);
						shader_->device_interface()->UnbindTextureResources(platform());
					}
				}

				vertex_buffer->Unbind(platform_);
				shader_->device_interface()->ClearVertexFormat();
			}
		}
	}

	void Renderer3DD3D11::DrawPrimitive(const  MeshInstance& mesh_instance, Int32 primitive_index, Int32 num_indices)
	{

	}
	void Renderer3DD3D11::SetFillMode(FillMode fill_mode)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());

		if (fill_mode == kWireframe)
		{
			platform_d3d.device_context()->RSSetState(wireframe_render_state_);
		}
		else
		{
			// default is solid
			platform_d3d.device_context()->RSSetState(default_render_state_);
		}
	}

	void Renderer3DD3D11::SetDepthTest(DepthTest depth_test)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());

		if (depth_test == kAlways)
		{
			platform_d3d.device_context()->OMSetDepthStencilState(always_depth_stencil_state_, 0);
		}
		else
		{
			// default
			platform_d3d.device_context()->OMSetDepthStencilState(default_depth_stencil_state_, 0);
		}
	}

}




#if 0
#include <platform/d3d11/graphics/renderer_3d_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/d3d11/graphics/default_3d_shader_d3d11.h>
#include <platform/d3d11/graphics/skinned_mesh_shader_d3d11.h>
#include <platform/d3d11/graphics/texture_d3d11.h>
//#include <platform/d3d11/graphics/mesh_d3d11.h>
//#include <platform/d3d11/graphics/primitive_d3d11.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <graphics/mesh_instance.h>
#include <graphics/colour.h>
#include <graphics/skinned_mesh_shader_data.h>
#include <graphics/vertex_buffer.h>
#include <graphics/index_buffer.h>
#include <graphics/material.h>

namespace gef
{
	class Platform;

	const D3D11_PRIMITIVE_TOPOLOGY Renderer3DD3D11::primitive_types[NUM_PRIMITIVE_TYPES] = 
	{
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,

	};

	Renderer3DD3D11::Renderer3DD3D11(Platform& platform) :
		Renderer3D(platform),
		default_render_state_(NULL),
		default_blend_state_(NULL),
		default_depth_stencil_state_(NULL),
		always_depth_stencil_state_(NULL),
		current_material_(NULL)
	{
		PointLight default_point_light;
		default_point_light.set_colour(Colour(0.7f, 0.7f, 1.0f, 1.0f));
		default_point_light.set_position(Vector3(-500.0f, 400.0f, 700.0f));
		default_shader_data_.set_ambient_light_colour(Colour(0.5f, 0.5f, 0.5f, 1.0f));
		default_shader_data_.AddPointLight(default_point_light);

		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);
		default_shader_ = new Default3DShaderD3D11(platform_d3d.device(), default_shader_data_);
		shader_ = default_shader_;


		default_skinned_mesh_shader_ = new SkinnedMeshShaderD3D11(platform_d3d.device(), default_skinned_mesh_shader_data_);

		default_texture_ = static_cast<TextureD3D11*>(Texture::CreateCheckerTexture(64, 8, platform));

		HRESULT hresult = S_OK;

		D3D11_RASTERIZER_DESC render_state_desc;
		ZeroMemory(&render_state_desc, sizeof(D3D11_RASTERIZER_DESC));
		render_state_desc.FillMode = D3D11_FILL_SOLID;
		render_state_desc.CullMode = D3D11_CULL_BACK;

		hresult = platform_d3d.device()->CreateRasterizerState(&render_state_desc, &default_render_state_);

		if(SUCCEEDED(hresult))
		{
			render_state_desc.FillMode = D3D11_FILL_WIREFRAME;
			hresult = platform_d3d.device()->CreateRasterizerState(&render_state_desc, &wireframe_render_state_);
		}

		if(SUCCEEDED(hresult))
		{

			D3D11_RENDER_TARGET_BLEND_DESC rtbd;
			ZeroMemory( &rtbd, sizeof(rtbd) );
			rtbd.BlendEnable = true;
			rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.BlendOp = D3D11_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbd.RenderTargetWriteMask = 0x0f;

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory( &blendDesc, sizeof(blendDesc) );
			blendDesc.RenderTarget[0] = rtbd;

			hresult = platform_d3d.device()->CreateBlendState(&blendDesc, &default_blend_state_);
		}

		if(SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			// Stencil test parameters
			dsDesc.StencilEnable = false;

			// Create depth stencil state
			platform_d3d.device()->CreateDepthStencilState(&dsDesc, &default_depth_stencil_state_);
		}

		if(SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			dsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil test parameters
			dsDesc.StencilEnable = false;

			// Create depth stencil state
			platform_d3d.device()->CreateDepthStencilState(&dsDesc, &always_depth_stencil_state_);
		}


		if(FAILED(hresult))
		{
			CleanUp();
		}
	}

	Renderer3DD3D11::~Renderer3DD3D11()
	{
		CleanUp();
	}

	void Renderer3DD3D11::CleanUp()
	{
		DeleteNull(default_texture_);
		ReleaseNull(default_render_state_);
		ReleaseNull(wireframe_render_state_);
		ReleaseNull(default_blend_state_);
		ReleaseNull(default_depth_stencil_state_);
		ReleaseNull(always_depth_stencil_state_);
	}


	void Renderer3DD3D11::Begin(bool clear)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
		platform_d3d.BeginScene();

		if(clear)
			platform_d3d.Clear();

		platform_d3d.device_context()->RSSetState(default_render_state_);
		platform_d3d.device_context()->OMSetBlendState(default_blend_state_, NULL, 0xffffffff);
		platform_d3d.device_context()->OMSetDepthStencilState(default_depth_stencil_state_, 0);
	}

	void Renderer3DD3D11::End()
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
		platform_d3d.EndScene();
	}

	void Renderer3DD3D11::DrawMesh(const class MeshInstance& mesh_instance)
	{
		const Mesh* mesh = mesh_instance.mesh();
		if(mesh != NULL)
		{
			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
			ShaderD3D11* shader_d3d = static_cast<ShaderD3D11*>(shader_);

			set_world_matrix(mesh_instance.transform());

			const VertexBuffer* vertex_buffer = mesh->vertex_buffer();
			if(vertex_buffer)
			{
				vertex_buffer->Bind(platform_);

				if(shader_d3d)
				{
					shader_d3d->Set(platform_d3d.device_context());
					//shader_d3d->SetConstantBuffers(platform_d3d.device_context(), this);
				}

				for(UInt32 primitive_index=0;primitive_index<mesh->num_primitives();++primitive_index)
				{
					const Primitive* primitive = mesh->GetPrimitive(primitive_index);

					// set the current material so it can be set in the shader data
					current_material_ = primitive->material();

					// shader parameters change per primitive
					if(shader_d3d)
						shader_d3d->SetConstantBuffers(platform_d3d.device_context(), this);

					const IndexBuffer* index_buffer = primitive->index_buffer();
					if(index_buffer)
					{
						// get the texture to use when rendering this primitive
						const TextureD3D11* texture = NULL;
						
						if(primitive->material())
							texture = static_cast<const TextureD3D11*>(primitive->material()->texture());
						if(!texture)
							texture = static_cast<const TextureD3D11*>(default_texture_);

						if(texture)
							texture->Bind(platform_d3d.device_context());

						// Set primitive topology
						platform_d3d.device_context()->IASetPrimitiveTopology( primitive_types[primitive->type()] );

						index_buffer->Bind(platform_);

						// use the primitive end index to specify how may indices we wish to draw
						// in case we don't want to draw them all
						if(index_buffer->num_indices() > 0)
							platform_d3d.device_context()->DrawIndexed(index_buffer->num_indices(), 0, 0);
						else
							platform_d3d.device_context()->Draw(vertex_buffer->num_vertices(), 0);

						index_buffer->Unbind(platform_);
					}
				}

				vertex_buffer->Unbind(platform_);
			}
		}
	}

	void Renderer3DD3D11::ClearZBuffer()
	{
	}

	void Renderer3DD3D11::SetFillMode(FillMode fill_mode)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());

		if(fill_mode == kWireframe)
		{
			platform_d3d.device_context()->RSSetState(wireframe_render_state_);
		}
		else
		{
			// default is solid
			platform_d3d.device_context()->RSSetState(default_render_state_);
		}
	}

	void Renderer3DD3D11::SetDepthTest(DepthTest depth_test)
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());

		if(depth_test == kAlways)
		{
			platform_d3d.device_context()->OMSetDepthStencilState(always_depth_stencil_state_, 0);
		}
		else
		{
			// default
			platform_d3d.device_context()->OMSetDepthStencilState(default_depth_stencil_state_, 0);
		}
	}

	void Renderer3DD3D11::DrawPrimitive(const  MeshInstance& mesh_instance, Int32 primitive_num, Int32 num_indices)
	{
		const Mesh* mesh = mesh_instance.mesh();
		if(mesh != NULL && num_indices > 0)
		{
			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform());
			ShaderD3D11* shader_d3d = static_cast<ShaderD3D11*>(shader_);

			set_world_matrix(mesh_instance.transform());

			const VertexBuffer* vertex_buffer = mesh->vertex_buffer();
			if(vertex_buffer)
			{
				vertex_buffer->Bind(platform_);

				if(shader_d3d)
					shader_d3d->Set(platform_d3d.device_context());

				//for(UInt32 primitive_index=0;primitive_index<mesh->num_primitives();++primitive_index)
				{
					const Primitive* primitive = mesh->GetPrimitive(primitive_num);

					// set the current material so it can be set in the shader data
					current_material_ = primitive->material();

					// shader parameters change per primitive
					if(shader_d3d)
						shader_d3d->SetConstantBuffers(platform_d3d.device_context(), this);

					// Set primitive topology
					platform_d3d.device_context()->IASetPrimitiveTopology( primitive_types[primitive->type()] );

					// get the texture to use when rendering this primitive
					const TextureD3D11* texture = NULL;
						
					if(primitive->material())
						texture = static_cast<const TextureD3D11*>(primitive->material()->texture());
					if(!texture)
						texture = static_cast<const TextureD3D11*>(default_texture_);

					if(texture)
						texture->Bind(platform_d3d.device_context());
						
					const IndexBuffer* index_buffer = primitive->index_buffer();
					if(index_buffer)
					{
						index_buffer->Bind(platform_);

						// use num_indices to specify how may indices we wish to draw
						// in case we don't want to draw them all
						platform_d3d.device_context()->DrawIndexed(num_indices < 0 ? index_buffer->num_indices() : num_indices, 0, 0);


						index_buffer->Unbind(platform_);
					}
					else
						platform_d3d.device_context()->Draw(num_indices < 0 ? vertex_buffer->num_vertices() : num_indices, 0);
				}

				vertex_buffer->Unbind(platform_);
			}
		}
	}


}
#endif