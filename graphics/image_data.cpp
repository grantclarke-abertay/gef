#include <graphics/image_data.h>
#include <cstdlib>

namespace gef
{
	ImageData::ImageData() :
		image_(NULL),
		clut_(NULL)
	{
	}

	ImageData::~ImageData()
	{
		delete image_;
		delete clut_;
	}
}