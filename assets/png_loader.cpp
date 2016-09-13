#include <assets/png_loader.h>
#include <system/file.h>
#include <system/platform.h>
#include <graphics/image_data.h>
#include <stdio.h>
#include <png.h>
//#include <pngstruct.h>
#include <stdlib.h>
#include <cstring>

typedef struct
{
    png_bytep p;
    png_uint_32 len;
} PNGData, *PNGDataPtr;

static void ReadDataFromInputStream(png_structp png_ptr, png_bytep data,
	png_size_t length)
{
	PNGDataPtr dataptr = (PNGDataPtr)png_get_io_ptr(png_ptr);

	if (length > dataptr->len)
	{
		png_error(png_ptr, "EOF");
		return;
	}

	memcpy(data, dataptr->p, length);


	dataptr->p += length;
	dataptr->len -= (png_uint_32)length;
}  // end ReadDataFromInputStream()

namespace gef
{

    PNGLoader::PNGLoader()
    {
    }

    PNGLoader::~PNGLoader()
    {
    }


    void PNGLoader::Load(const char* filename, const Platform& platform, ImageData& image_data)
    {
        File* png_file   = gef::File::Create();

        bool success = png_file->Open(filename);
        if(success)
        {
            // read entire file
            UInt8* buffer = NULL;
            Int32 file_size;
            success = png_file->GetSize(file_size);
            if(success)
            {
                buffer = new UInt8[file_size];
                Int32 bytes_read;
                success = png_file->Read(buffer, file_size, bytes_read);

                if(success)
                    success = png_file->Close();

                if(success)
                {
                    png_structp png_ptr;
                    png_infop info_ptr;
                    UInt32 sig_read = 0;
                    png_uint_32 width = 0;
                    png_uint_32 height = 0;
                    int bitDepth = 0;
                    int colorType = -1;


                    /* Create and initialize the png_struct
                     * with the desired error handler
                     * functions.  If you want to use the
                     * default stderr and longjump method,
                     * you can supply NULL for the last
                     * three parameters.  We also supply the
                     * the compiler header file version, so
                     * that we know if the application
                     * was compiled with a compatible version
                     * of the library.  REQUIRED
                     */
                    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,	NULL, NULL, NULL);

                    if(png_ptr == NULL)
                    {
                        success = false;
                        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                        delete[] buffer;
                        buffer = NULL;
                    }

                    if(success)
                    {
                        /* Allocate/initialize the memory
                         * for image information.  REQUIRED. */
                        info_ptr = png_create_info_struct(png_ptr);
                        if (info_ptr == NULL)
                        {
                            success = false;
                            delete[] buffer;
                            buffer = NULL;
                            png_destroy_read_struct(&png_ptr, NULL, NULL);
                        }
                    }

                    if(success)
                    {
                        /* Set error handling if you are
                         * using the setjmp/longjmp method
                         * (this is the normal method of
                         * doing things with libpng).
                         * REQUIRED unless you  set up
                         * your own error handlers in
                         * the png_create_read_struct()
                         * earlier.
                         */
                        if (setjmp(png_jmpbuf(png_ptr)))
                        {
                            success = false;
                            delete[] buffer;
                            buffer = NULL;

                            /* Free all of the memory associated
                             * with the png_ptr and info_ptr */
                            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                        }
                    }

                    if(success)
                    {
						PNGData data;
						data.p = buffer;
						data.len = file_size;

                        png_set_read_fn(png_ptr, &data, ReadDataFromInputStream);

                        /* If we have already
                         * read some of the signature */
                        png_set_sig_bytes(png_ptr, sig_read);

                        png_read_info(png_ptr, info_ptr);

                        png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,
                                &width,
                                &height,
                                &bitDepth,
                                &colorType,
                                NULL, NULL, NULL);

                        if(retval != 1)
                        {
                            success = false;
                            delete[] buffer;
                            buffer = NULL;

                            /* Free all of the memory associated
                             * with the png_ptr and info_ptr */
                            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                        }
                    }

                    if(success)
                    {
                        if(colorType == PNG_COLOR_TYPE_RGB)
                        {
                            png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
                            png_read_update_info(png_ptr, info_ptr);

                            png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,
                                    &width,
                                    &height,
                                    &bitDepth,
                                    &colorType,
                                    NULL, NULL, NULL);

                            if(retval != 1)
                            {
                                success = false;
                                delete[] buffer;
                                buffer = NULL;

                                /* Free all of the memory associated
                                 * with the png_ptr and info_ptr */
                                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                            }

                        }
                    }

                    if(success)
                    {
                        image_data.set_width(width);
                        image_data.set_height(height);
                        UInt32 row_bytes = (UInt32)png_get_rowbytes(png_ptr, info_ptr);
                        UInt8* image_bytes = (UInt8*)malloc(row_bytes * height);
                        image_data.set_image(image_bytes);

                        switch(colorType)
                        {
                            case PNG_COLOR_TYPE_RGB:
                                //      ParseRGB(outImage, png_ptr, info_ptr);
                                break;

                            case PNG_COLOR_TYPE_RGB_ALPHA:
                                //outHasAlpha = true;
                                ParseRGBA(image_data.image(), png_ptr, info_ptr, width, height);
                                break;
                        }
                    }
                }

                // done
                delete[] buffer;
                buffer = NULL;
            }
        }
    }

    void PNGLoader::ParseRGBA(UInt8* out_image_buffer, void* the_png_ptr,
            const void* the_info_ptr, UInt32 width, UInt32 height)
    {
        png_structp png_ptr = (png_structp)the_png_ptr;
        png_infop info_ptr = (png_infop)the_info_ptr;

        const png_uint_32 bytesPerRow = (png_uint_32)png_get_rowbytes(png_ptr, info_ptr);
        UInt8* rowData = new UInt8[bytesPerRow];
        const UInt32 kBytesPerPixel = bytesPerRow / width;

        // read single row at a time
        for(UInt32 rowIdx = 0; rowIdx < height; ++rowIdx)
        {
            png_read_row(png_ptr, (png_bytep)rowData, NULL);

            const UInt32 rowOffset = rowIdx * width;
            UInt32 pixel_byte_offset = rowOffset*kBytesPerPixel;
            memcpy(&out_image_buffer[pixel_byte_offset], rowData, width*kBytesPerPixel);
        }

        delete [] rowData;

    }  // end ParseRGBA()

    UInt32 PNGLoader::NextPowerOfTwo(const UInt32 value)
    {
        // http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
        UInt32 result = value;

        result--;
        result |= result >> 1;
        result |= result >> 2;
        result |= result >> 4;
        result |= result >> 8;
        result |= result >> 16;
        result++;

        return result;
    }

}
