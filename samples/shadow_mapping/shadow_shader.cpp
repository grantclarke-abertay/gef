/*
* default_3d_shader.cpp
*
*  Created on: 29 Jan 2015
*      Author: grant
*/

#include "shadow_shader.h"
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <system/debug_log.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
#include <graphics/colour.h>
#include <graphics/default_3d_shader_data.h>

	ShadowShader::ShadowShader(const gef::Platform& platform)
		:Shader(platform)
		, wvp_matrix_variable_index_(-1)
		, world_matrix_variable_index_(-1)
		, invworld_matrix_variable_index_(-1)
		, light_wvp_matrix_variable_index_(-1)
		, light_position_variable_index_(-1)
		, material_colour_variable_index_(-1)
		, ambient_light_colour_variable_index_(-1)
		, light_colour_variable_index_(-1)
		, texture_sampler_index_(-1)
	{
		bool success = true;

		// load vertex shader source in from a file
		char* vs_shader_source = NULL;
		Int32 vs_shader_source_length = 0;
		success = LoadShader("shadow_shader_vs", "shaders/shadow_mapping", &vs_shader_source, vs_shader_source_length, platform);

		char* ps_shader_source = NULL;
		Int32 ps_shader_source_length = 0;
		success = LoadShader("shadow_shader_ps", "shaders/shadow_mapping", &ps_shader_source, ps_shader_source_length, platform);

		device_interface_->SetVertexShaderSource(vs_shader_source, vs_shader_source_length);
		device_interface_->SetPixelShaderSource(ps_shader_source, ps_shader_source_length);

		delete[] vs_shader_source;
		vs_shader_source = NULL;
		delete[] ps_shader_source;
		ps_shader_source = NULL;

		wvp_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("wvp", gef::ShaderInterface::kMatrix44);
		world_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("world", gef::ShaderInterface::kMatrix44);
		invworld_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("invworld", gef::ShaderInterface::kMatrix44);
		light_wvp_matrix_variable_index_ = device_interface_->AddVertexShaderVariable("lightwvp", gef::ShaderInterface::kMatrix44);
		light_position_variable_index_ = device_interface_->AddVertexShaderVariable("light_position", gef::ShaderInterface::kVector4, 4);

		// pixel shader variables
		// TODO - probable need to keep these separate for D3D11
		material_colour_variable_index_ = device_interface_->AddPixelShaderVariable("material_colour", gef::ShaderInterface::kVector4);
		ambient_light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("ambient_light_colour", gef::ShaderInterface::kVector4);
		light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("light_colour", gef::ShaderInterface::kVector4, 4);

		texture_sampler_index_ = device_interface_->AddTextureSampler("texture_sampler");
		depth_sampler_index_ = device_interface_->AddTextureSampler("depth_sampler");

		device_interface_->AddVertexParameter("position", gef::ShaderInterface::kVector3, 0, "POSITION", 0);
		device_interface_->AddVertexParameter("normal", gef::ShaderInterface::kVector3, 12, "NORMAL", 0);
		device_interface_->AddVertexParameter("uv", gef::ShaderInterface::kVector2, 24, "TEXCOORD", 0);
		device_interface_->set_vertex_size(sizeof(gef::Mesh::Vertex));
		device_interface_->CreateVertexFormat();

		success = device_interface_->CreateProgram();
	}

	ShadowShader::~ShadowShader()
	{
	}



	void ShadowShader::SetMeshData(
		const gef::Default3DShaderData& shader_data,
		const gef::Matrix44& world_matrix,
		const gef::Matrix44& inv_world_matrix,
		const gef::Matrix44& view_matrix,
		const gef::Matrix44& projection_matrix,
		const gef::Matrix44& light_view_matrix,
		const gef::Matrix44& light_projection_matrix)
	{
		gef::Matrix44 wvp = world_matrix * view_matrix * projection_matrix;
		gef::Matrix44 light_wvp = world_matrix * light_view_matrix * light_projection_matrix;
		gef::Vector4 light_positions[MAX_NUM_POINT_LIGHTS];
		gef::Vector4 light_colours[MAX_NUM_POINT_LIGHTS];
		gef::Vector4 ambient_light_colour = shader_data.ambient_light_colour().GetRGBAasVector4();

		for (Int32 light_num = 0; light_num < MAX_NUM_POINT_LIGHTS; ++light_num)
		{
			gef::Vector4 light_position;
			gef::Colour light_colour;
			if (light_num < shader_data.GetNumPointLights())
			{
				const gef::PointLight& point_light = shader_data.GetPointLight(light_num);
				light_position = point_light.position();
				light_colour = point_light.colour();
			}
			else
			{
				// no light data
				// set this light to a light with no colour
				light_position = gef::Vector4(0.0f, 0.0f, 0.0f);
				light_colour = gef::Colour(0.0f, 0.0f, 0.0f);
			}
			light_positions[light_num] = gef::Vector4(light_position.x(), light_position.y(), light_position.z(), 1.f);
			light_colours[light_num] = light_colour.GetRGBAasVector4();
		}


		gef::Matrix44 wvpT, worldT, invworldT, light_wvpT;
		wvpT.Transpose(wvp);
		worldT.Transpose(world_matrix);
		invworldT.Transpose(inv_world_matrix);
		light_wvpT.Transpose(light_wvp);
		device_interface_->SetVertexShaderVariable(wvp_matrix_variable_index_, &wvpT);
		device_interface_->SetVertexShaderVariable(world_matrix_variable_index_, &worldT);
		device_interface_->SetVertexShaderVariable(invworld_matrix_variable_index_, &invworldT);
		device_interface_->SetVertexShaderVariable(light_wvp_matrix_variable_index_, &light_wvpT);
		device_interface_->SetVertexShaderVariable(light_position_variable_index_, (float*)light_positions);

		device_interface_->SetPixelShaderVariable(ambient_light_colour_variable_index_, (float*)&ambient_light_colour);
		device_interface_->SetPixelShaderVariable(light_colour_variable_index_, (float*)light_colours);
	}

	void ShadowShader::SetMaterialData(const gef::Material* material)
	{
		gef::Colour material_colour(1.0f, 1.0f, 1.0f, 1.0f);
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
		device_interface_->SetTextureSampler(depth_sampler_index_, primitive_data_.depth_texture);
	}
