#include <graphics/vertex_buffer.h>
#include <stdlib.h>

namespace gef
{
	VertexBuffer::VertexBuffer() :
		num_vertices_(0),
		vertex_byte_size_(0),
		vertex_data_(NULL)
	{
	}

	VertexBuffer::~VertexBuffer()
	{
		free(vertex_data_);
	}
}