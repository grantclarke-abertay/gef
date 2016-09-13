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
	virtual ~Texture();
	virtual void Bind(const Platform& platform, const int texture_stage_num) const = 0;
	virtual void Unbind(const Platform& platform, const int texture_stage_num) const = 0;

	static Texture* Create(const Platform& platform, const ImageData& image_data);
	static Texture* CreateCheckerTexture(const Int32 size, const Int32 num_checkers, const Platform& platform);

protected:
	Texture();
	Texture(const Platform& platform, const ImageData& image_data);

};

}
#endif // _GEF_TEXTURE_H
