/*
 * default_3d_shader.cpp
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#include <graphics/default_3d_skinning_shader.h>
#include <graphics/shader_interface.h>
#include <graphics/mesh_instance.h>
#include <graphics/primitive.h>
#include <system/debug_log.h>
#include <graphics/mesh.h>
#include <graphics/material.h>
#include <graphics/colour.h>
#include <graphics/skinned_mesh_shader_data.h>

namespace gef
{
	Default3DSkinningShader::Default3DSkinningShader(const Platform& platform)
	:Shader(platform)
	,wvp_matrix_variable_index_(-1)
	,world_matrix_variable_index_(-1)
	,invworld_matrix_variable_index_(-1)
	,light_position_variable_index_(-1)
	,material_colour_variable_index_(-1)
	,ambient_light_colour_variable_index_(-1)
	,light_colour_variable_index_(-1)
	,texture_sampler_index_(-1)
	,bone_matrices_variable_index_(-1)
	{
		bool success = true;

		// load vertex shader source in from a file
		char* vs_shader_source = NULL;
		Int32 vs_shader_source_length = 0;
		success = LoadShader("default_3d_skinning_shader_vs", "shaders/gef", &vs_shader_source, vs_shader_source_length, platform);

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
		bone_matrices_variable_index_ = device_interface_->AddVertexShaderVariable("bone_matrices", ShaderInterface::kMatrix44, 128);

		// pixel shader variables
		// TODO - probable need to keep these separate for D3D11
		material_colour_variable_index_ = device_interface_->AddPixelShaderVariable("material_colour", ShaderInterface::kVector4);
		ambient_light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("ambient_light_colour", ShaderInterface::kVector4);
		light_colour_variable_index_ = device_interface_->AddPixelShaderVariable("light_colour", ShaderInterface::kVector4, 4);

		texture_sampler_index_ = device_interface_->AddTextureSampler("texture_sampler");

		device_interface_->AddVertexParameter("position", ShaderInterface::kVector3, 0, "POSITION", 0);
		device_interface_->AddVertexParameter("normal", ShaderInterface::kVector3, 12, "NORMAL", 0);
		device_interface_->AddVertexParameter("bone_indices", ShaderInterface::kUByte4, 24, "BLENDINDICES", 0);
		device_interface_->AddVertexParameter("bone_weights", ShaderInterface::kVector4, 28, "BLENDWEIGHT", 0);
		device_interface_->AddVertexParameter("uv", ShaderInterface::kVector2, 44, "TEXCOORD", 0);
		device_interface_->set_vertex_size(sizeof(Mesh::SkinnedVertex));
		device_interface_->CreateVertexFormat();

		success = device_interface_->CreateProgram();
	}

	Default3DSkinningShader::Default3DSkinningShader()
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

	Default3DSkinningShader::~Default3DSkinningShader()
	{
	}



	void Default3DSkinningShader::SetSceneData(const SkinnedMeshShaderData& shader_data, const Matrix44& view_matrix, const Matrix44& projection_matrix)
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

		// need to transpose the bone matrices for the shader
		if (bone_matrices_variable_index_ != -1)
		{
			UInt32 matrix_index = 0;
			for (std::vector<gef::Matrix44>::const_iterator matrix_iter = shader_data.bone_matrices()->begin(); matrix_iter != shader_data.bone_matrices()->end(); ++matrix_iter, ++matrix_index)
				mesh_data_.bones_matrices[matrix_index].Transpose((*matrix_iter));

			device_interface_->SetVertexShaderVariable(bone_matrices_variable_index_, (float*)&mesh_data_.bones_matrices[0], (Int32)shader_data.bone_matrices()->size());
		}

		device_interface_->SetPixelShaderVariable(ambient_light_colour_variable_index_, (float*)&ambient_light_colour);
		device_interface_->SetPixelShaderVariable(light_colour_variable_index_, (float*)light_colours);
	}

	void Default3DSkinningShader::SetMeshData(const gef::MeshInstance& mesh_instance)
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

	void Default3DSkinningShader::SetMaterialData(const gef::Material* material)
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
