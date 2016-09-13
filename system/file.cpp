#include <system/file.h>
#include <stdlib.h>
#include <system/debug_log.h>

namespace gef
{
	File::File()
	{

	}

	File::~File()
	{
	}


	bool File::Load(const char* const filename, void** buffer, Int32& buffer_size)
	{
		bool success = Open(filename);
		if(!success)
		{
			DebugOut("File::Load: %s: file failed to open", filename);
			exit(-1);
		}


		Int32 file_size = 0;
		success = GetSize(file_size);
		if(!success)
		{
			DebugOut("File::Load: %s: failed to get file size", filename);
			exit(-1);
		}

		*buffer = std::malloc(file_size);
		if(!*buffer)
		{
			DebugOut("File::Load: %s: failed to allocate buffer for contents", filename);
			exit(-1);
		}
		buffer_size = file_size;

		Int32 bytes_read = 0;
		success = Read(*buffer, file_size, bytes_read);

		if(!success)
		{
			DebugOut("File::Load: %s: failed to read % bytes into buffer", filename, file_size);
			exit(-1);
		}

		buffer_size = file_size;

		Close();

		return success;
	}

}
