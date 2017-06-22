/*
 * default_3d_shader.cpp
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#include <graphics/default_3d_shader.h>
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <system/debug_log.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
#include <graphics/colour.h>
#include <graphics/default_3d_shader_data.h>

#ifdef _WIN32
#include <platform/d3d11/graphics/shader_interface_d3d11.h>
#endif


namespace gef
{
	Default3DShader::Default3DShader(const Platform& platform)
	:Shader(platform)
	,wvp_matrix_variable_index_(-1)
	,world_matrix_variable_index_(-1)
	,invworld_matrix_variable_index_(-1)
	,light_position_variable_index_(-1)
	,material_colour_variable_index_(-1)
	,ambient_light_colour_variable_index_(-1)
	,light_colour_variable_index_(-1)
	,texture_sampler_index_(-1)
	{
		bool success = true;

		// load vertex shader source in from a file
		char* vs_shader_source = NULL;
		Int32 vs_shader_source_length = 0;
		success = LoadShader("default_3d_shader_vs", "shaders/gef", &vs_shader_source, vs_shader_source_length, platform);

		char* ps_shader_source = NULL;
		Int32 ps_shader_source_length = 0;
		success = LoadShader("default_3d_shader_ps", "shaders/gef", &ps_shader_source, ps_shader_source_length, platform);

		device_interface_->SetVertexShaderSource(vs_shader_source, vs_shader_source_length);
		device_interface_->SetPixelShaderSource(ps_shader_source, ps_shader_source_length);

		delete[] vs_shader_source;
		vs_shader_source = NULL;
		delete[] ps_shader_source;
		ps_shader_source = NULL;

		wvp_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("wvp", ShaderInterface::kMatrix44);
		world_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("world", ShaderInterface::kMatrix44);
		invworld_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("invworld", ShaderInterface::kMatrix44);
		light_position_variable_index_ = device_interface_->AddVertexShaderVariable("light_position", ShaderInterface::kVector4, 4);

		// pixel shader variables
		// TODO - probable need to keep these separate for D3D11
		material_colour_variable_index_ = device_interface_->AddPixelShaderVariable("material_colour", ShaderInterface::kVector4);
		ambient_light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("ambient_light_colour", ShaderInterface::kVector4);
		light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("light_colour", ShaderInterface::kVector4, 4);

		texture_sampler_index_ = device_interface_->AddTextureSampler("texture_sampler");

		device_interface_->AddVertexParameter("position", ShaderInterface::kVector3, 0, "POSITION", 0);
		device_interface_->AddVertexParameter("normal", ShaderInterface::kVector3, 12, "NORMAL", 0);
		device_interface_->AddVertexParameter("uv", ShaderInterface::kVector2, 24, "TEXCOORD", 0);
		device_interface_->set_vertex_size(sizeof(Mesh::Vertex));
		device_interface_->CreateVertexFormat();

#ifdef _WIN32
		gef::ShaderInterfaceD3D11* shader_interface_d3d11 = static_cast<gef::ShaderInterfaceD3D11*>(device_interface_);

		// Create a texture sampler state description.
		D3D11_SAMPLER_DESC sampler_desc;
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		sampler_desc.MipLODBias = 0.0f;
		sampler_desc.MaxAnisotropy = 1;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

		shader_interface_d3d11->AddSamplerState(sampler_desc);
#endif

		success = device_interface_->CreateProgram();
	}

	Default3DShader::Default3DShader()
		: wvp_matrix_variable_index_(-1)
		, world_matrix_variable_index_(-1)
		, invworld_matrix_variable_index_(-1)
		, light_position_variable_index_(-1)
		, material_colour_variable_index_(-1)
		, ambient_light_colour_variable_index_(-1)
		, light_colour_variable_index_(-1)
		, texture_sampler_index_(-1)
	{

	}

	Default3DShader::~Default3DShader()
	{
	}



	void Default3DShader::SetSceneData(const Default3DShaderData& shader_data, const Matrix44& view_matrix, const Matrix44& projection_matrix)
	{
		//gef::Matrix44 wvp = world_matrix * view_matrix * projection_matrix;
		gef::Vector4 light_positions[MAX_NUM_POINT_LIGHTS];
		gef::Vector4 light_colours[MAX_NUM_POINT_LIGHTS];
		gef::Vector4 ambient_light_colour = shader_data.ambient_light_colour().GetRGBAasVector4();

		for (Int32 light_num = 0; light_num < MAX_NUM_POINT_LIGHTS; ++light_num)
		{
			Vector4 light_position;
			Colour light_colour;
			if (light_num < shader_data.GetNumPointLights())
			{
				const PointLight& point_light = shader_data.GetPointLight(light_num);
				light_position = point_light.position();
				light_colour = point_light.colour();
			}
			else
			{
				// no light data
				// set this light to a light with no colour
				light_position = Vector4(0.0f, 0.0f, 0.0f);
				light_colour = Colour(0.0f, 0.0f, 0.0f);
			}
			light_positions[light_num] = Vector4(light_position.x(), light_position.y(), light_position.z(), 1.f);
			light_colours[light_num] = light_colour.GetRGBAasVector4();
		}

		view_projection_matrix_ = view_matrix * projection_matrix;


		device_interface_->SetVertexShaderVariable(light_position_variable_index_, (float*)light_positions);
		device_interface_->SetPixelShaderVariable(ambient_light_colour_variable_index_, (float*)&ambient_light_colour);
		device_interface_->SetPixelShaderVariable(light_colour_variable_index_, (float*)light_colours);
	}

	void Default3DShader::SetMeshData(const gef::MeshInstance& mesh_instance)
	{
		// calculate world view projection matrix
		gef::Matrix44 wvp = mesh_instance.transform() * view_projection_matrix_;

		// calculate the transpose of inverse world matrix to transform normals in shader
		Matrix44 inv_world;
		inv_world.Inverse(mesh_instance.transform());
		//inv_world_transpose_matrix.Transpose(inv_world);

		// take transpose of matrices for the shaders
		gef::Matrix44 wvpT, worldT;

		wvpT.Transpose(wvp);
		worldT.Transpose(mesh_instance.transform());
		// taking the transpose of the inverse world transpose matrix, just give use the inverse world matrix
		// no need to waste calculating that here

		device_interface_->SetVertexShaderVariable(wvp_matrix_variable_index_, &wvpT);
		device_interface_->SetVertexShaderVariable(world_matrix_variable_index_, &worldT);
		device_interface_->SetVertexShaderVariable(invworld_matrix_variable_index_, &inv_world);
	}

	void Default3DShader::SetMaterialData(const gef::Material* material)
	{
		Colour material_colour(1.0f, 1.0f, 1.0f, 1.0f);
		if (material)
		{
			primitive_data_.material_texture = material->texture();
			material_colour.SetFromAGBR(material->colour());
		}
		else
			primitive_data_.material_texture = NULL;

		primitive_data_.material_colour = material_colour.GetRGBAasVector4();


		device_interface_->SetPixelShaderVariable(material_colour_variable_index_, (float*)&primitive_data_.material_colour);
		device_interface_->SetTextureSampler(texture_sampler_index_, primitive_data_.material_texture);
	}

} /* namespace gef */
