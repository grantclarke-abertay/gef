#ifndef _GEF_VERTEX_BUFFER_D3D11_H
#define _GEF_VERTEX_BUFFER_D3D11_H

#include <graphics/vertex_buffer.h>
#include <d3d11.h>

namespace gef
{
	class VertexBufferD3D11 : public VertexBuffer
	{
	public:
		VertexBufferD3D11();
		~VertexBufferD3D11();
		bool Init(const Platform& platform, const void* vertices, const UInt32 num_vertices, const UInt32 vertex_byte_size, const bool read_only = true);
		bool Update(const Platform& platform);

		void Bind(const Platform& platform) const;
		void Unbind(const Platform& platform) const;
	private:
		ID3D11Buffer* vertex_buffer_;
	};
}

#endif // _GEF_VERTEX_BUFFER_D3D11_H
