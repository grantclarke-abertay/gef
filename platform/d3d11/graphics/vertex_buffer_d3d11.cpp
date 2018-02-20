#include <platform/d3d11/graphics/vertex_buffer_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>
#include <gef.h>

namespace gef
{
	VertexBuffer*  VertexBuffer::Create(Platform& platform)
	{
		return new VertexBufferD3D11();
	}

	VertexBufferD3D11::VertexBufferD3D11() :
		vertex_buffer_(NULL)
	{
	}

	VertexBufferD3D11::~VertexBufferD3D11()
	{
		ReleaseNull(vertex_buffer_);
	}

	bool VertexBufferD3D11::Init(const Platform& platform, const void* vertices, const UInt32 num_vertices, const UInt32 vertex_byte_size, const bool read_only)
	{
		num_vertices_ = num_vertices;
		vertex_byte_size_ = vertex_byte_size;
		bool success = true;
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);

		if (!read_only)
		{
			// take a copy of the vertex data
			vertex_data_ = malloc(vertex_byte_size * num_vertices);
			if (!vertex_data_)
				success = false;
			else
			{
				if (vertices)
					memcpy(vertex_data_, vertices, vertex_byte_size * num_vertices);
				else
				{
					// if vertices ptr is null
					// then assign vertex_data_ to vertices
					// so we can create a vertex buffer with unintialised data
					// that can be written to later
					vertices = vertex_data_;
				}
			}
		}

		if (success)
		{
			D3D11_BUFFER_DESC bd;
			// if copy vertex data is set then we're assuming this buffer will be getting updated, so let's make it dynamic
			bd.Usage = read_only ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = vertex_byte_size * num_vertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = read_only ? 0 : D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA init_data;
			init_data.pSysMem = vertices;

			HRESULT hresult = platform_d3d.device()->CreateBuffer(&bd, &init_data, &vertex_buffer_);
			if (FAILED(hresult))
			{
				success = false;
			}
		}



		return success;
	}

	bool VertexBufferD3D11::Update(const Platform& platform)
	{
		bool success = true;
		if (vertex_data_ && vertex_buffer_)
		{

			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);


			D3D11_MAPPED_SUBRESOURCE resource;
			HRESULT result = platform_d3d.device_context()->Map(vertex_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			if (result == S_OK)
			{
				resource.pData;
				memcpy(resource.pData, vertex_data_, num_vertices()*vertex_byte_size());
				platform_d3d.device_context()->Unmap(vertex_buffer_, 0);
			}
		}
		success = false;
		return success;
	}

	void VertexBufferD3D11::Bind(const Platform& platform) const
	{
		const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);
		UINT stride = vertex_byte_size_;
		UINT offset = 0;
		platform_d3d.device_context()->IASetVertexBuffers(0, 1, &vertex_buffer_, &stride, &offset);
	}

	void VertexBufferD3D11::Unbind(const Platform& platform) const
	{
		// don't need to unbind buffer?
	}
}