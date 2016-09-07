#ifndef _GEF_IMAGE_DATA_H
#define _GEF_IMAGE_DATA_H

#include <gef.h>

namespace gef
{
	class ImageData
	{
	public:
		ImageData();
		~ImageData();

		UInt8* image() const { return image_; }
		void set_image(UInt8* const image) { image_ = image; }
		const UInt8* clut() const { return clut_; }
		void set_clut(UInt8* const clut) { clut_ = clut; }
		const UInt32 width() const { return width_; }
		void set_width(const UInt32 width) { width_ = width; }
		const UInt32 height() const { return height_; }
		void set_height(const UInt32 height) { height_ = height; }

	private:
		UInt8* image_;
		UInt8* clut_;
		UInt32 width_;
		UInt32 height_;
	};
}


#endif // _GEF_IMAGE_DATA_H
