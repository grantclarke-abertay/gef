/*
 * shader_interface.h
 *
 *  Created on: 28 Jan 2015
 *      Author: grant
 */

#ifndef GRAPHICS_SHADER_INTERFACE_H_
#define GRAPHICS_SHADER_INTERFACE_H_

#include <gef.h>
#include <string>
#include <vector>

namespace gef
{
	class Texture;
	class Platform;

	class ShaderInterface
	{
	public:
		enum VariableType
		{
			kFloat = 0,
			kMatrix44,
			kVector2,
			kVector3,
			kVector4,
			kUByte4
//			kNumParameterTypes
		};

		struct ShaderVariable
		{
			std::string name;
			VariableType type;
			Int32 byte_offset;
			Int32 count;
		};

		struct ShaderParameter
		{
			std::string name;
			VariableType type;
			Int32 byte_offset;
			std::string semantic_name;
			Int32 semantic_index;
		};

		struct TextureSampler
		{
			std::string name;
			const Texture* texture;
		};

		virtual ~ShaderInterface();

		void SetVertexShaderSource(const char* vs_shader_source, Int32 vs_shader_source_size);
		void SetPixelShaderSource(const char* ps_shader_source, Int32 ps_shader_source_size);


		virtual bool CreateProgram() = 0;
		virtual void CreateVertexFormat() = 0;

		void AddVertexParameter(const char* parameter_name, VariableType variable_type, Int32 byte_offset, const char* semantic_name, int semantic_index);
		inline void set_vertex_size(Int32 vertex_size) {vertex_size_ = vertex_size; }

		Int32 AddVertexShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count = 1);
		void SetVertexShaderVariable(Int32 variable_index, const void* value, Int32 variable_count = -1);
		Int32 AddPixelShaderVariable(const char* variable_name, VariableType variable_type, Int32 variable_count = 1);
		void SetPixelShaderVariable(Int32 variable_index, const void* value);

		Int32 AddTextureSampler(const char* texture_sampler_name);
		void SetTextureSampler(Int32 texture_sampler_index, const Texture* texture);

		virtual void UseProgram() = 0;

		virtual void SetVariableData() = 0;
		virtual void SetVertexFormat() = 0;
		virtual void ClearVertexFormat() = 0;

		virtual void BindTextureResources(const Platform& platform) const = 0;
		virtual void UnbindTextureResources(const Platform& platform) const = 0;

		static ShaderInterface* Create(const Platform& platform);

	protected:
		ShaderInterface();

		static Int32 GetTypeSize(VariableType type);

		Int32 AddVariable(std::vector<ShaderVariable>& variables, const char* variable_name, VariableType variable_type, Int32 variable_count);
		virtual void SetVariable(std::vector<ShaderVariable>& variables, UInt8* variables_data, Int32 variable_index, const void* value, Int32 variable_count = -1);
		void AllocateVariableData();
		UInt8* AllocateVariableData(std::vector<ShaderVariable>& variables, Int32& variable_data_size);

		char* vs_shader_source_;
		Int32 vs_shader_source_size_;
		char* ps_shader_source_;
		Int32 ps_shader_source_size_;

		std::vector<ShaderParameter> parameters_;
		std::vector<ShaderVariable> vertex_shader_variables_;
		std::vector<ShaderVariable> pixel_shader_variables_;
        std::vector<TextureSampler> texture_samplers_;
		UInt8* vertex_shader_variable_data_;
		Int32 vertex_shader_variable_data_size_;
		UInt8* pixel_shader_variable_data_;
		Int32 pixel_shader_variable_data_size_;
		Int32 vertex_size_;
	};
}



#endif /* GRAPHICS_SHADER_INTERFACE_H_ */
