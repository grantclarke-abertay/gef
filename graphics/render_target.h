#ifndef _GEF_RENDER_TARGET_H
#define _GEF_RENDER_TARGET_H

#include <gef.h>

namespace gef
{
	class Texture;
	class Platform;

	class RenderTarget
	{
	public:
		virtual ~RenderTarget();

		virtual void Begin(const Platform& platform) = 0;
		virtual void End(const Platform& platform) = 0;

		inline Int32 width() const { return width_; }
		inline Int32 height() const { return height_; }
		inline const Texture* texture() const { return texture_; }

		static RenderTarget* Create(const Platform& platform, const Int32 width, const Int32 height);
	protected:
		RenderTarget(const Platform& platform, const Int32 width, const Int32 height);
		Int32 width_;
		Int32 height_;
		Texture* texture_;
	};
}

#endif // _GEF_RENDER_TARGET_H
