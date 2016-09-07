#include <graphics/render_target.h>
#include <graphics/texture.h>
#include <cstddef>

namespace gef
{
	RenderTarget::RenderTarget(const Platform& platform, const Int32 width, const Int32 height) :
		texture_(NULL)
	{
		width_ = width;
		height_ = height;
	}

	RenderTarget::~RenderTarget()
	{
		delete texture_;
	}
}