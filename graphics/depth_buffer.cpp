#include <graphics/depth_buffer.h>
#include <graphics/texture.h>
#include <cstddef>

namespace gef
{
	DepthBuffer::DepthBuffer(UInt32 height, UInt32 width) :
		height_(height),
		width_(width),
		texture_(NULL)
	{

	}

	DepthBuffer::~DepthBuffer()
	{
		delete texture_;
	}
}