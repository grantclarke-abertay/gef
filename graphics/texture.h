#ifndef _GEF_TEXTURE_H
#define _GEF_TEXTURE_H

#include <gef.h>

namespace gef
{
	class ImageData;
	class Platform;

class Texture
{
public:
	Texture();
	Texture(const Platform& platform, const ImageData& image_data);
	virtual ~Texture();

	virtual void Bind(const Platform& platform, const int texture_stage_num) const = 0;
	virtual void Unbind(const Platform& platform, const int texture_stage_num) const = 0;

	static Texture* CreateCheckerTexture(const Int32 size, const Int32 num_checkers, const Platform& platform);
};

}
#endif // _GEF_TEXTURE_H
