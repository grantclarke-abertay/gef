/*
 * shader_interface.cpp
 *
 *  Created on: 28 Jan 2015
 *      Author: grant
 */

#include <graphics/shader_interface.h>
#include <cstdlib>
#include <cstring>

namespace gef
{
	ShaderInterface::ShaderInterface() 
#if 1
        :	vertex_shader_variable_data_(NULL),
			vertex_shader_variable_data_size_(0),
			pixel_shader_variable_data_(NULL),
			pixel_shader_variable_data_size_(0),
			vertex_size_(0),
			vs_shader_source_(NULL),
			vs_shader_source_size_(0),
			ps_shader_source_(NULL),
			ps_shader_source_size_(0)
#endif
	{
	}
	ShaderInterface::~ShaderInterface()
	{
#if 1
		free(vertex_shader_variable_data_);
		free(pixel_shader_variable_data_);
		free(vs_shader_source_);
		free(ps_shader_source_);
#endif
	}
#if 1
	Int32 ShaderInterface::AddVertexShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		return AddVariable(vertex_shader_variables_, variable_name, variable_type, variable_count);
	}

	void ShaderInterface::SetVertexShaderVariable(Int32 variable_index, const void* value)
	{
		SetVariable(vertex_shader_variables_, vertex_shader_variable_data_, variable_index, value);
	}

	Int32 ShaderInterface::AddPixelShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		return AddVariable(pixel_shader_variables_, variable_name, variable_type, variable_count);
	}

	void ShaderInterface::SetPixelShaderVariable(Int32 variable_index, const void* value)
	{
		SetVariable(pixel_shader_variables_, pixel_shader_variable_data_, variable_index, value);
	}

	Int32 ShaderInterface::AddVariable(std::vector<ShaderVariable>& variables, const char* variable_name, VariableType variable_type, Int32 variable_count)
	{
		ShaderVariable shader_variable;
		shader_variable.name = variable_name;
		shader_variable.type = variable_type;
		shader_variable.byte_offset = 0;
		shader_variable.count = variable_count;
		variables.push_back(shader_variable);
		return (Int32)variables.size()-1;
	}

	void ShaderInterface::SetVariable(std::vector<ShaderVariable>& variables, UInt8* variables_data, Int32 variable_index, const void* value)
	{
		ShaderVariable& shader_variable = variables[variable_index];
		void* variable_data = &static_cast<UInt8*>(variables_data)[shader_variable.byte_offset];
		Int32 data_size = GetTypeSize(shader_variable.type)*shader_variable.count;
		memcpy(variable_data, value, data_size);
	}


	void ShaderInterface::AddVertexParameter(const char* parameter_name, VariableType parameter_type, Int32 parameter_byte_offset, const char* semantic_name, int semantic_index)
	{
		ShaderParameter shader_parameter;
		shader_parameter.name = parameter_name;
		shader_parameter.type = parameter_type;
		shader_parameter.byte_offset = parameter_byte_offset;
		shader_parameter.semantic_name = semantic_name;
		shader_parameter.semantic_index = semantic_index;
		parameters_.push_back(shader_parameter);
	}

	Int32 ShaderInterface::AddTextureSampler(const char* texture_sampler_name)
	{
		TextureSampler texture_sampler;
		texture_sampler.name = texture_sampler_name;
		texture_sampler.texture = NULL;
		texture_samplers_.push_back(texture_sampler);
		return (Int32)texture_samplers_.size()-1;
	}

	void ShaderInterface::SetTextureSampler(Int32 texture_sampler_index, const Texture* texture)
	{
		TextureSampler& texture_sampler = texture_samplers_[texture_sampler_index];
		texture_sampler.texture = texture;
	}

	Int32 ShaderInterface::GetTypeSize(VariableType type)
	{
		Int32 size;
		switch(type)
		{
		case kFloat:
			size = 4;
			break;
		case kVector2:
			size = 8;
			break;
		case kVector3:
			size = 12;
			break;
		case kVector4:
			size = 16;
			break;
		case kMatrix44:
			size = 64;
			break;
		}

		return size;
	}

	void ShaderInterface::AllocateVariableData()
	{
		vertex_shader_variable_data_ = AllocateVariableData(vertex_shader_variables_, vertex_shader_variable_data_size_);
		pixel_shader_variable_data_ = AllocateVariableData(pixel_shader_variables_, pixel_shader_variable_data_size_);
	}


	UInt8* ShaderInterface::AllocateVariableData(std::vector<ShaderVariable>& variables, Int32& variable_data_size)
	{
		variable_data_size = 0;
		for(std::vector<ShaderVariable>::iterator shader_variable = variables.begin(); shader_variable != variables.end(); ++shader_variable)
		{
			shader_variable->byte_offset = variable_data_size;
			variable_data_size += GetTypeSize(shader_variable->type)*shader_variable->count;
		}

		return static_cast<UInt8*>(malloc(variable_data_size));
	}

	void ShaderInterface::SetVertexShaderSource(const char* vs_shader_source, Int32 vs_shader_source_size)
	{
		vs_shader_source_size_ = vs_shader_source_size;
		vs_shader_source_ = new char[vs_shader_source_size];
		memcpy((void*)vs_shader_source_, vs_shader_source, vs_shader_source_size_);
	}

	void ShaderInterface::SetPixelShaderSource(const char* ps_shader_source, Int32 ps_shader_source_size)
	{
		ps_shader_source_size_ = ps_shader_source_size;
		ps_shader_source_ = new char[ps_shader_source_size];
		memcpy((void*)ps_shader_source_, ps_shader_source, ps_shader_source_size_);
	}

#endif
}
