#ifndef _DEPTH_SHADER_H
#define _DEPTH_SHADER_H

#include <graphics/shader.h>
#include <maths/matrix44.h>

namespace gef
{
	class MeshInstance;
	class Primitive;
	class Platform;
}

class DepthShader : public gef::Shader
{
public:
	struct VertexShaderData
	{
		gef::Matrix44 wvp;
	};


	DepthShader(const gef::Platform& platform);
	virtual ~DepthShader();

	void SetVertexShaderData(const gef::Matrix44& world_matrix, const gef::Matrix44& view_matrix, const gef::Matrix44& projection_matrix);
protected:

	Int32 wvp_matrix_variable_index_;
};

#endif // _DEPTH_SHADER_H