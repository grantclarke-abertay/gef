#ifndef _GEF_FILE_H
#define _GEF_FILE_H

#include <cstdlib>
#include <gef.h>

namespace gef
{
	enum SeekFrom
	{
		SF_Start = 0,
		SF_Current,
		SF_End
	};

	class File
	{
	public:

		virtual ~File();

		virtual bool Open(const char* const filename) = 0;
		virtual bool Exists(const char* const filename) = 0;

		virtual bool Seek(const SeekFrom seek_from, Int32 offset/*, Int32* position = NULL*/) = 0;
		virtual bool Read(void *buffer, const Int32 size, Int32& bytes_read) = 0;
//		virtual bool Read(void *buffer, const Int32 size, const Int32 offset, Int32& bytes_read) =  0;
		virtual bool Close() = 0;
		virtual bool GetSize(Int32 &size) = 0;
		bool Load(const char* const filename, void** buffer, Int32& buffer_size);

		static File* Create();
	protected:
		File();
	};
}

#endif // _GEF_FILE_H
