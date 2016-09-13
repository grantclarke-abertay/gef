#include <graphics/primitive.h>
#include <graphics/index_buffer.h>
#include <system/platform.h>
#include <cstdlib>
#include <assert.h>

namespace gef
{

	Primitive::Primitive(Platform& platform) :
		material_(NULL),
		type_(UNDEFINED),
		index_buffer_(NULL),
		platform_(platform)
	{
	}

	bool Primitive::InitIndexBuffer(Platform& platform, const void* indices, const UInt32 num_indices, const UInt32 index_byte_size,const bool read_only)
	{
		assert(index_buffer_ == NULL);
		index_buffer_ = gef::IndexBuffer::Create(platform);
		platform_.AddIndexBuffer(index_buffer_);
		return index_buffer_->Init(platform, indices, num_indices, index_byte_size, read_only);
	}

	Primitive::~Primitive()
	{
		if(index_buffer_)
		{
			platform_.RemoveIndexBuffer(index_buffer_);
			delete index_buffer_;
		}
	}

}