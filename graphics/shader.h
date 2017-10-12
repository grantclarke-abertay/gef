#ifndef _GEF_SHADER_H
#define _GEF_SHADER_H

#include <string>
#include <gef.h>

namespace gef
{
	class Platform;
	class ShaderInterface;
	class MeshInstance;
	class Primitive;
	class Material;
	class Matrix44;

	class Shader
	{
	public:
		Shader(const Platform& platform);
		Shader();
		virtual ~Shader();
		virtual void SetVertexFormat();
		//virtual void SetData(const void* data);
		virtual void SetMeshData(const gef::MeshInstance& mesh_instance);
		virtual void SetMeshData(const gef::Matrix44& transform);
		virtual void SetMaterialData(const gef::Material* material);

		inline ShaderInterface* device_interface() { return device_interface_; }
	protected:
		bool LoadShader(const char* filename, const char* base_filepath, char** shader_source, Int32& shader_source_length, const Platform& platform);
		ShaderInterface* device_interface_;
	};
}


#endif // _GEF_SHADER_H
