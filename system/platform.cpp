#include <system/platform.h>
#include <input/touch_input_manager.h>
#include <graphics/render_target.h>
#include <graphics/mesh.h>

namespace gef
{
	Platform::Platform() :
		render_target_(NULL),
		touch_input_manager_(NULL),
		default_texture_(NULL),
		depth_buffer_(NULL)
	{
		set_render_target_clear_colour(Colour(0.4f, 0.525f, 0.7f, 1.0f));
	}

	Platform::~Platform()
	{
		DeleteNull(touch_input_manager_);
	}

	std::string Platform::FormatFilename(const std::string& filename) const
	{
		return FormatFilename(filename.c_str());
	}

	RenderTarget* Platform::CreateRenderTarget() const
	{
		return gef::RenderTarget::Create(*this, width(), height());
	}

	bool Platform::ReadyToRender() const
	{
		return true;
	}

	void Platform::AddShader(Shader* shader)
	{
		shaders_.push_back(shader);
	}

	void Platform::RemoveShader(Shader* shader)
	{
		shaders_.remove(shader);
	}

	void Platform::AddTexture(Texture* texture)
	{
		textures_.push_back(texture);
	}

	void Platform::RemoveTexture(Texture* texture)
	{
		textures_.remove(texture);
	}

	void Platform::AddVertexBuffer(VertexBuffer* vertex_buffer)
	{
		vertex_buffers_.push_back(vertex_buffer);
	}

	void Platform::RemoveVertexBuffer(VertexBuffer* vertex_buffer)
	{
		vertex_buffers_.remove(vertex_buffer);
	}

	void Platform::AddIndexBuffer(IndexBuffer* index_buffer)
	{
		index_buffers_.push_back(index_buffer);
	}

	void Platform::RemoveIndexBuffer(IndexBuffer* index_buffer)
	{
		index_buffers_.remove(index_buffer);
	}

	Mesh* Platform::CreateMesh()
	{
		return new Mesh(*this);
	}


}
