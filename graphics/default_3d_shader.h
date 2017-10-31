/*
 * default_3d_shader.h
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#ifndef GRAPHICS_DEFAULT_3D_SHADER_H_
#define GRAPHICS_DEFAULT_3D_SHADER_H_

#include <graphics/shader.h>
#include <gef.h>
#include <maths/vector4.h>
#include <maths/matrix44.h>

#define MAX_NUM_POINT_LIGHTS 4

namespace gef
{
	class MeshInstance;
	class Matrix44;
	class Primitive;
	class Texture;
	class Material;
	class Default3DShaderData;

	class Default3DShader: public Shader
	{
	public:
		struct MeshData
		{
			Matrix44 wvp;
			Matrix44 world;
			Matrix44 invworld;
			Vector4 ambient_light_colour;
			Vector4 light_position[MAX_NUM_POINT_LIGHTS];
			Vector4 light_colour[MAX_NUM_POINT_LIGHTS];
		};

		struct PrimitiveData
		{
			Vector4 material_colour;
			const gef::Texture* material_texture;
		};

		Default3DShader(const Platform& platform);
		virtual ~Default3DShader();
		//void SetSceneData(const Matrix44& wvp_matrix);
		//void SetSpriteData(const Sprite& sprite, const Texture* texture);
		void SetSceneData(const Default3DShaderData& shader_data, const Matrix44& view_matrix, const Matrix44& projection_matrix);
		void SetMeshData(const gef::MeshInstance& mesh_instance);
		void SetMeshData(const gef::Matrix44& transform);
		void SetMaterialData(const gef::Material* material);

		inline PrimitiveData& primitive_data() { return primitive_data_; }
	protected:
		Default3DShader();

		Int32 wvp_matrix_variable_index_;
		Int32 world_matrix_variable_index_;
//		Int32 invworld_matrix_variable_index_;
		Int32 light_position_variable_index_;

		Int32 material_colour_variable_index_;
		Int32 ambient_light_colour_variable_index_;
		Int32 light_colour_variable_index_;

		Int32 texture_sampler_index_;

		MeshData mesh_data_;
		PrimitiveData primitive_data_;

		gef::Matrix44 view_projection_matrix_;

	};

} /* namespace gef */

#endif /* GRAPHICS_DEFAULT_3D_SHADER_H_ */
