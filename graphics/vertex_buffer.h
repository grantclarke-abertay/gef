#ifndef _GEF_VERTEX_BUFFER_H
#define _GEF_VERTEX_BUFFER_H

#include <gef.h>

namespace gef
{
	class Platform;

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer();
		virtual bool Init(const Platform& platform, const void* vertices, const UInt32 num_vertices, const UInt32 vertex_byte_size, const bool read_only = true) = 0;
		virtual bool Update(const Platform& platform) = 0;

		virtual void Bind(const Platform& platform) const = 0;
		virtual void Unbind(const Platform& platform) const = 0;

		inline UInt32 num_vertices() const { return num_vertices_; }
		inline UInt32 vertex_byte_size() const { return vertex_byte_size_; }

		void* vertex_data() {return vertex_data_; }

		static VertexBuffer* Create(Platform& platform);

	protected:
		VertexBuffer();
		UInt32 num_vertices_;
		UInt32 vertex_byte_size_;
		void* vertex_data_;
	};
}

#endif // _GEF_VERTEX_BUFFER_H
