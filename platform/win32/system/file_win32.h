#ifndef _GEF_FILE_WIN32_H
#define _GEF_FILE_WIN32_H

#include <system/file.h>
#include <Windows.h>

namespace gef
{

class FileWin32 : public File
{
public:

	FileWin32();
	~FileWin32();

	bool Open(const char* const filename);
	bool Exists(const char* const filename);
	bool Seek(const SeekFrom seek_from, Int32 offset/*, Int32* position = NULL*/);
	bool Read(void *buffer, const Int32 size, Int32& bytes_read);
	bool Read(void *buffer, const Int32 size, const Int32 offset, Int32& bytes_read);
	bool Close();
	bool GetSize(Int32 &size);




private:
	HANDLE file_descriptor_;

	static const HANDLE kInvalidHandle;
};

}

#endif // _GEF_FILE_WIN32_H
