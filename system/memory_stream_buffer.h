#ifndef _GEF_MEMORY_STREAM_BUFFER_H
#define _GEF_MEMORY_STREAM_BUFFER_H

#include <streambuf>

namespace gef
{
	class MemoryStreamBuffer : public std::streambuf
	{
	public:
		MemoryStreamBuffer(char* buffer, size_t size);
	};
}

#endif // _GEF_MEMORY_STREAM_BUFFER_H
