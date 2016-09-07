#ifndef _GEF_INDEX_BUFFER_D3D11_H
#define _GEF_INDEX_BUFFER_D3D11_H

#include <graphics/index_buffer.h>
#include <d3d11.h>

namespace gef
{
	class IndexBufferD3D11 : public IndexBuffer
	{
	public:
		IndexBufferD3D11();
		~IndexBufferD3D11();

		bool Init(const Platform& platform, const void* indices, const UInt32 num_indices, const UInt32 index_byte_size, const bool read_only = true);
		void Bind(const Platform& platform) const;
		void Unbind(const Platform& platform) const;
		bool Update(const Platform& platform);
	private:
		ID3D11Buffer*      index_buffer_;
		DXGI_FORMAT format_;
	};
}

#endif // _GEF_INDEX_BUFFER_D3D11_H
