#ifndef _SHADOW_SHADER_H
#define _SHADOW_SHADER_H

#include <graphics/shader.h>
#include <maths/matrix44.h>
#include <maths/vector4.h>


#define MAX_NUM_POINT_LIGHTS 4

namespace gef
{
	class MeshInstance;
	class Material;
	class Default3DShaderData;
	class Texture;
}

class ShadowShader : public gef::Shader
{
public:
	struct MeshData
	{
		gef::Matrix44 wvp;
		gef::Matrix44 world;
		gef::Matrix44 invworld;
		gef::Vector4 ambient_light_colour;
		gef::Vector4 light_position[MAX_NUM_POINT_LIGHTS];
		gef::Vector4 light_colour[MAX_NUM_POINT_LIGHTS];
	};

	struct PrimitiveData
	{
		gef::Vector4 material_colour;
		const gef::Texture* material_texture;
		const gef::Texture* depth_texture;
	};

	ShadowShader(const gef::Platform& platform);
	virtual ~ShadowShader();

	void SetMeshData(
		const gef::Default3DShaderData& shader_data,
		const gef::Matrix44& world_matrix,
		const gef::Matrix44& inv_world_matrix,
		const gef::Matrix44& view_matrix,
		const gef::Matrix44& projection_matrix,
		const gef::Matrix44& light_view_matrix,
		const gef::Matrix44& light_projection_matrix);
	void SetMaterialData(const gef::Material* material);

	inline PrimitiveData& primitive_data() { return primitive_data_; }
protected:

	Int32 wvp_matrix_variable_index_;
	Int32 world_matrix_variable_index_;
	Int32 invworld_matrix_variable_index_;
	Int32 light_wvp_matrix_variable_index_;
	Int32 light_position_variable_index_;

	Int32 material_colour_variable_index_;
	Int32 ambient_light_colour_variable_index_;
	Int32 light_colour_variable_index_;

	Int32 texture_sampler_index_;
	Int32 depth_sampler_index_;

	MeshData mesh_data_;
	PrimitiveData primitive_data_;

};


#endif // _SHADOW_SHADER_H