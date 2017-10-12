#include <platform/win32/system/file_win32.h>

#include <cstdio>
#include <Windows.h>

namespace gef
{
	File* File::Create()
	{
		return new FileWin32();
	}

	const HANDLE FileWin32::kInvalidHandle = NULL;

	FileWin32::FileWin32() :
		file_descriptor_(kInvalidHandle)
	{
	}

	FileWin32::~FileWin32()
	{
		// make sure file descriptor is invalid
		// if it's not then the file has not been closed properly
	//	SCE_DBG_ASSERT(file_descriptor_ == kInvalidHandle);
	}

	bool FileWin32::Open(const char* const filename)
	{
		file_descriptor_ = CreateFile(filename,
			GENERIC_READ,
			FILE_SHARE_READ,                      // do not share
			NULL,                   // default security
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,  // normal file
			NULL);                  // no attr. template

		if (file_descriptor_ != INVALID_HANDLE_VALUE)
			return true;
		else
			return false;
	}

	bool FileWin32::Exists(const char* const filename)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE handle = FindFirstFile(filename, &FindFileData);
		bool found = handle != INVALID_HANDLE_VALUE;
		if (found)
		{
			//FindClose(&handle); this will crash
			FindClose(handle);
		}
		return found ;
	}


	bool FileWin32::Close()
	{
		bool success = true;
		if (file_descriptor_ >= 0)
		{
			CloseHandle(file_descriptor_);
			file_descriptor_ = kInvalidHandle;
		}

		return success;
	}

	bool FileWin32::GetSize(Int32 &size)
	{
		LARGE_INTEGER file_size;
		if (GetFileSizeEx(file_descriptor_, &file_size) < 0)
			return false;

		size = static_cast<Int32>(file_size.LowPart);

		return true;
	}

	bool FileWin32::Seek(const SeekFrom seek_from, const Int32 offset/*, Int32* position*/)
	{
		DWORD from = FILE_BEGIN;
		switch (seek_from)
		{
		case SF_Start:
			from = FILE_BEGIN;
			break;
		case SF_Current:
			from = FILE_CURRENT;
			break;
		case SF_End:
			from = FILE_END;
			break;
		}

		LARGE_INTEGER large_offset;
		large_offset.QuadPart = offset;
		BOOL return_value = SetFilePointerEx(file_descriptor_, large_offset, NULL, from);

		//	if(position)
		//		*position = static_cast<Int32>(return_value);

		return return_value ? true : false;
	}

	bool FileWin32::Read(void *buffer, const Int32 size, Int32& bytes_read)
	{
		DWORD dword_bytes_read;
		BOOL return_value = ReadFile(file_descriptor_, buffer, size, &dword_bytes_read, NULL);

		bytes_read = dword_bytes_read;
		return return_value ? true : false;
	}

	bool FileWin32::Read(void *buffer, const Int32 size, const Int32 offset, Int32& bytes_read)
	{
		LARGE_INTEGER large_offset;
		large_offset.QuadPart = offset;
		BOOL return_value = SetFilePointerEx(file_descriptor_, large_offset, NULL, FILE_BEGIN);

		DWORD dword_bytes_read;
		if (return_value)
			return_value = ReadFile(file_descriptor_, buffer, size, &dword_bytes_read, NULL);

		bytes_read = return_value;
		return return_value ? true : false;
	}

}