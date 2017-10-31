/*
 * file_std.cpp
 *
 *  Created on: 27 Jan 2015
 *      Author: grant
 */

#include "file_std.h"
#include <sys/stat.h>



namespace gef
{
    gef::File* gef::File::Create()
    {
        return new gef::FileStd();
    }

	FileStd::FileStd()
	:file_handle_(NULL)
	{

	}

	FileStd::~FileStd()
	{
		if(file_handle_)
			Close();
	}

	bool FileStd::Open(const char* const filename)
	{
		file_handle_ = fopen(filename, "rb");
		return file_handle_ != NULL;
	}

	bool FileStd::Seek(const SeekFrom seek_from, Int32 offset/*, Int32* position = NULL*/)
	{
		int from = SEEK_SET;
		switch(seek_from)
		{
		case SF_Start:
			from = SEEK_SET;
			break;
		case SF_End:
			from = SEEK_END;
			break;
		case SF_Current:
			from = SEEK_CUR;
			break;
		}

		return fseek(file_handle_, offset, from) == 0;
	}

	bool FileStd::Read(void *buffer, const Int32 size, Int32& bytes_read)
	{
		bytes_read = fread(buffer, 1, size, file_handle_);
		return bytes_read == size;
	}

	bool FileStd::Close()
	{
		bool success = fclose(file_handle_) == 0;
		file_handle_ = NULL;
		return success;
	}

	bool FileStd::GetSize(Int32 &size)
	{
		int current_position = ftell(file_handle_);
		if(fseek(file_handle_, 0, SEEK_END) == 0)
		{
			size = ftell(file_handle_);
			// set file pointer back to current position
			if(fseek(file_handle_, current_position, SEEK_SET) == 0)
				return true;
			else
				return false;
		}
		else
			return false;
	}

    bool FileStd::Exists(const char *const filename)
    {
        struct stat buffer;
        return (stat (filename, &buffer) == 0);
    }

}
