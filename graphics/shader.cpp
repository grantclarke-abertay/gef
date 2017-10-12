#include <graphics/shader.h>
#include <system/platform.h>
#include <system/debug_log.h>
#include <graphics/shader_interface.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <istream>
#include <iterator>
#include <cstring>

namespace gef
{
	Shader::Shader(const Platform& platform)
		:device_interface_(NULL)
	{
		device_interface_ = gef::ShaderInterface::Create(platform);
	}

	Shader::Shader() :
		device_interface_(NULL)
	{
	}

	Shader::~Shader()
	{
		delete device_interface_;
	}

	//void Shader::SetData(const void* data)
	//{
	//}

	void Shader::SetVertexFormat()
	{
	}

	void Shader::SetMeshData(const gef::MeshInstance& mesh_instance)
	{

	}

	void Shader::SetMeshData(const gef::Matrix44& transform)
	{

	}


	void Shader::SetMaterialData(const gef::Material* material)
	{

	}


	bool Shader::LoadShader(const char* filename, const char* base_filepath, char** shader_source, Int32& shader_source_length, const Platform& platform)
	{
		File* vs_file = gef::File::Create();
		void* buffer = NULL;
		Int32 buffer_size = 0;

		std::string full_filepath = std::string(base_filepath) + "/"+ std::string(platform.GetShaderDirectory()) + "/" + std::string(filename) + "." + std::string(platform.GetShaderFileExtension());

		bool success = vs_file->Load(full_filepath.c_str(), &buffer, buffer_size);
		if(!success)
		{
			DebugOut("LoadShader: %s failed to load\n", filename);
			exit(-1);
		}

		shader_source_length = buffer_size;
		*shader_source = new char[shader_source_length];
		memcpy(*shader_source, buffer, shader_source_length);

		return success;
	}
}
