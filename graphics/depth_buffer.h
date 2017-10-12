#ifndef _GEF_GRAPHICS_DEPTH_BUFFER_H
#define _GEF_GRAPHICS_DEPTH_BUFFER_H

#include <gef.h>

namespace gef
{
	class Platform;
	class Texture;

	class DepthBuffer
	{
	public:
		virtual ~DepthBuffer();

		inline UInt32 width() const { return width_; }
		inline UInt32 height() const { return height_; }
		inline const Texture* texture() const { return texture_; }

		static DepthBuffer* Create(const Platform& platform, UInt32 width, UInt32 height);
	protected:
		DepthBuffer(UInt32 width, UInt32 height);

		UInt32 width_;
		UInt32 height_;

		Texture* texture_;


	};
}

#endif // _GEF_GRAPHICS_DEPTH_BUFFER_H
