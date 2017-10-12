#include <platform/d3d11/graphics/index_buffer_d3d11.h>
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	IndexBuffer* IndexBuffer::Create(Platform& platform)
	{
		return new IndexBufferD3D11();
	}

	IndexBufferD3D11::IndexBufferD3D11() :
		index_buffer_(NULL),
		format_(DXGI_FORMAT_UNKNOWN)
	{
	}

	IndexBufferD3D11::~IndexBufferD3D11()
	{
		ReleaseNull(index_buffer_);
	}

	bool IndexBufferD3D11::Init(const Platform& platform, const void* indices, const UInt32 num_indices, const UInt32 index_byte_size, const bool read_only)
	{
		num_indices_ = num_indices;
		index_byte_size_ = index_byte_size;

		bool success = true;


		if (!read_only)
		{
			// take a copy of the vertex data
			index_data_ = malloc(index_byte_size_ * num_indices_);
			if (!index_data_)
				success = false;
			else
				if(indices)
					memcpy(index_data_, indices, index_byte_size_ * num_indices_);
				else
				{
					// if indices ptr is null
					// then assign index_data_ to indices
					// so we can create a buffer with unintialised data
					// that can be written to later
					indices = index_data_;
				}

		}

		if (success)
		{
			switch (index_byte_size_)
			{
			case 1:
				format_ = DXGI_FORMAT_R8_UINT;
				break;
			case 2:
				format_ = DXGI_FORMAT_R16_UINT;
				break;
			case 4:
				format_ = DXGI_FORMAT_R32_UINT;
				break;

			default:
				break;
			}

			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);

			D3D11_BUFFER_DESC bd;
			bd.Usage = read_only ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = index_byte_size * num_indices_;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = read_only ? 0 : D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA init_data;
			init_data.pSysMem = indices;
			HRESULT hresult = platform_d3d.device()->CreateBuffer(&bd, &init_data, &index_buffer_);
			if (FAILED(hresult))
			{
				success = false;
			}
		}

		return success;
	}

	void IndexBufferD3D11::Bind(const Platform& platform) const
	{
		if(index_buffer_)
		{
			// Set index buffer
			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);
			platform_d3d.device_context()->IASetIndexBuffer( index_buffer_, format_, 0 );
		}
	}

	void IndexBufferD3D11::Unbind(const Platform& platform) const
	{
		// no need to unbind buffer?
	}

	bool IndexBufferD3D11::Update(const Platform& platform)
	{
		bool success = true;
		if(index_data_)
		{

			const PlatformD3D11& platform_d3d = static_cast<const PlatformD3D11&>(platform);


			D3D11_MAPPED_SUBRESOURCE resource;
			HRESULT result = platform_d3d.device_context()->Map(index_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
			if(result == S_OK)
			{
				resource.pData;
				memcpy(resource.pData, index_data_, num_indices()*index_byte_size());
				platform_d3d.device_context()->Unmap(index_buffer_, 0);
			}
		}
		success = false;
		return success;	
	}
}