#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <gef.h>

namespace gef
{
	class Texture;

	class Material
	{
	public:
		Material();
		virtual ~Material();

		inline void set_texture(Texture* texture) { texture_  = texture; }
		inline const Texture* texture() const { return texture_; }
		inline void set_colour(UInt32 abgr) { colour_ = abgr;}
		inline UInt32 colour() const { return colour_; }

	private:
		class Texture* texture_;
		UInt32 colour_;
	};
}

#endif // _MATERIAL_H