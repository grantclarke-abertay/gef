#ifndef _GEF_PNG_LOADER_H
#define _GEF_PNG_LOADER_H

#include <gef.h>


namespace gef
{
	// forward declarations
	class Platform;
	class ImageData;

	class PNGLoader
	{
	public:
		PNGLoader();
		~PNGLoader();

		void Load(const char* filename, const Platform& platform, ImageData& image_data);
	private:
		void ParseRGBA(UInt8* out_image_buffer, void* png_ptr, const void* info_ptr, UInt32 width, UInt32 height);
		UInt32 NextPowerOfTwo(const UInt32 value);
	};

}

#endif // _GEF_PNG_LOADER_H
