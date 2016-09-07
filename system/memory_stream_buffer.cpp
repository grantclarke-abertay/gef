#include <system/memory_stream_buffer.h>

namespace gef
{
	MemoryStreamBuffer::MemoryStreamBuffer(char* buffer, size_t size)
	{
		setg(buffer, buffer, buffer + size);
		setp(buffer, buffer + size);
	}
}
