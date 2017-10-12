#include <platform/d3d11/graphics/shader_interface_d3d11.h>
#include <d3dcompiler.h>
#include <graphics/texture.h>
#include <platform/d3d11/system/platform_d3d11.h>

namespace gef
{
	ShaderInterface* ShaderInterface::Create(const Platform& platform)
	{
		const PlatformD3D11& platform_d3d11 = reinterpret_cast<const PlatformD3D11&>(platform);
		return new ShaderInterfaceD3D11(platform_d3d11.device(), platform_d3d11.device_context());
	}

	ShaderInterfaceD3D11::ShaderInterfaceD3D11(ID3D11Device* device, ID3D11DeviceContext* device_context)
		: device_(device)
		, device_context_(device_context)
		, vertex_input_layout_(NULL)
		, vertex_shader_(NULL)
		, pixel_shader_(NULL)
		, elements_(NULL)
		, num_elements_(0)
		, vs_constant_buffer_(NULL)
		, ps_constant_buffer_(NULL)
	{
	}

	ShaderInterfaceD3D11::~ShaderInterfaceD3D11()
	{
		ReleaseNull(vertex_input_layout_);
		ReleaseNull(pixel_shader_);
		ReleaseNull(vertex_shader_);
		DeleteNull(elements_);

		for (auto sampler_state = sampler_states_.begin(); sampler_state != sampler_states_.end(); ++sampler_state)
			ReleaseNull((*sampler_state));

		ReleaseNull(vs_constant_buffer_);
		ReleaseNull(ps_constant_buffer_);
	}

	bool ShaderInterfaceD3D11::CreateProgram()
	{
		//
		// allocate memory to store local copy of variable data
		//
		AllocateVariableData();
//		CreateSamplerStates();
		CreateVertexShaderConstantBuffer();
		CreatePixelShaderConstantBuffer();

		bool success = true;
		// Compile and create the vertex shader
		if (vs_shader_source_size_ > 0)
		{


			DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
			shader_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			ID3D10Blob* vs_blob = NULL;
			ID3D10Blob* error_blob = NULL;
			HRESULT hresult = S_OK;
			hresult = D3DCompile(vs_shader_source_, vs_shader_source_size_, /*vertex_shader_source_name*/ NULL, NULL, NULL, "VS",
				"vs_4_0", shader_flags, 0, &vs_blob, &error_blob);
			if (FAILED(hresult))
			{
				if (error_blob != NULL)
				{
					OutputDebugStringA((CHAR*)error_blob->GetBufferPointer());
					error_blob->Release();
				}
			}
			else
			{
				hresult = device_->CreateVertexShader(vs_blob->GetBufferPointer(), vs_blob->GetBufferSize(),
					NULL, &vertex_shader_);
			}


			// create the vertex input layout
			if (SUCCEEDED(hresult))
			{
				hresult = device_->CreateInputLayout(elements_, num_elements_, vs_blob->GetBufferPointer(),
					vs_blob->GetBufferSize(), &vertex_input_layout_);
			}


			if (FAILED(hresult))
			{
				success = false;
				ReleaseNull(vertex_shader_);
			}
			ReleaseNull(error_blob);
			ReleaseNull(vs_blob);
		}


		// Compile and create the pixel shader
		if (success && (ps_shader_source_size_ > 0))
		{
			DWORD shader_flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
			shader_flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			ID3D10Blob* ps_blob = NULL;
			ID3D10Blob* error_blob = NULL;
			HRESULT hresult = S_OK;
			hresult = D3DCompile(ps_shader_source_, ps_shader_source_size_, /*pixel_shader_source_name*/NULL, NULL, NULL, "PS",
				"ps_4_0", shader_flags, 0, &ps_blob, &error_blob);
			if (FAILED(hresult))
			{
				if (error_blob != NULL)
				{
					OutputDebugStringA((CHAR*)error_blob->GetBufferPointer());
					error_blob->Release();
				}
			}
			else
			{
				hresult = device_->CreatePixelShader(ps_blob->GetBufferPointer(), ps_blob->GetBufferSize(),
					NULL, &pixel_shader_);
			}

			if (FAILED(hresult))
			{
				success = false;
				ReleaseNull(vertex_shader_);
				ReleaseNull(vertex_input_layout_);
				ReleaseNull(pixel_shader_);
			}
			ReleaseNull(error_blob);
			ReleaseNull(ps_blob);
		}

		return success;
	}

	void ShaderInterfaceD3D11::CreateVertexFormat()
	{
		// create elements for input assembly
		num_elements_ = (Int32)parameters_.size();
		elements_ = new D3D11_INPUT_ELEMENT_DESC[num_elements_];
		for (Int32 element_num = 0; element_num < num_elements_; ++element_num)
			SetInputAssemblyElement(parameters_[element_num], elements_[element_num]);
	}


	void ShaderInterfaceD3D11::UseProgram()
	{

		device_context_->VSSetShader(this->vertex_shader_, NULL, 0);
		device_context_->PSSetShader(this->pixel_shader_, NULL, 0);
	}

	void ShaderInterfaceD3D11::SetVariableData()
	{
		HRESULT hresult = S_OK;
		D3D11_MAPPED_SUBRESOURCE mappedResource;


		void* vs_data = NULL;
		void* ps_data = NULL;

		if (vs_constant_buffer_)
		{
			// Lock the constant buffer so it can be written to.
			hresult = device_context_->Map(vs_constant_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// Get a pointer to the data in the constant buffer.
			if (SUCCEEDED(hresult))
				vs_data = mappedResource.pData;
		}

		if (SUCCEEDED(hresult))
		{
			if (ps_constant_buffer_)
			{
				hresult = device_context_->Map(ps_constant_buffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				if (SUCCEEDED(hresult))
					ps_data = mappedResource.pData;
			}
		}

		if (SUCCEEDED(hresult))
		{
			if (vs_data)
				memcpy(vs_data, vertex_shader_variable_data_, vertex_shader_variable_data_size_);
			if (ps_data)
				memcpy(ps_data, pixel_shader_variable_data_, pixel_shader_variable_data_size_);

			// Unlock the constant buffer.
			if (vs_constant_buffer_)
				device_context_->Unmap(vs_constant_buffer_, 0);
			if (ps_constant_buffer_)
				device_context_->Unmap(ps_constant_buffer_, 0);

			// Set the position of the constant buffer in the vertex shader.
			UInt32 buffer_number = 0;

			// Now set the constant buffer in the shader with the updated values.
			if (vs_constant_buffer_)
				device_context_->VSSetConstantBuffers(buffer_number, 1, &vs_constant_buffer_);
			if (ps_constant_buffer_)
				device_context_->PSSetConstantBuffers(buffer_number, 1, &ps_constant_buffer_);
		}
	}

	void ShaderInterfaceD3D11::SetVertexFormat()
	{
		device_context_->IASetInputLayout(vertex_input_layout_);
	}

	void ShaderInterfaceD3D11::ClearVertexFormat()
	{

	}

	void ShaderInterfaceD3D11::BindTextureResources(const Platform& platform) const
	{
		if (sampler_states_.size() > 0)
			device_context_->PSSetSamplers(0, (UINT)sampler_states_.size(), &sampler_states_[0]);


		Int32 texture_stage_num = 0;
		for (auto texture_sampler = texture_samplers_.begin(); texture_sampler != texture_samplers_.end(); ++texture_sampler, ++texture_stage_num)
		{
			if (texture_sampler->texture)
				texture_sampler->texture->Bind(platform, texture_stage_num);
			else if (platform.default_texture())
				platform.default_texture()->Bind(platform, texture_stage_num);
				
		}

	}

	void ShaderInterfaceD3D11::UnbindTextureResources(const Platform& platform) const
	{
		Int32 texture_stage_num = 0;
		for (auto texture_sampler = texture_samplers_.begin(); texture_sampler != texture_samplers_.end(); ++texture_sampler, ++texture_stage_num)
		{
			if (texture_sampler->texture)
				texture_sampler->texture->Unbind(platform, texture_stage_num);
		}
	}

	void ShaderInterfaceD3D11::SetInputAssemblyElement(const ShaderParameter& shader_parameter, D3D11_INPUT_ELEMENT_DESC& element)
	{
		element.SemanticName = shader_parameter.semantic_name.c_str();
		element.SemanticIndex = shader_parameter.semantic_index;
		element.Format = GetVertexAttributeFormat(shader_parameter.type);
		element.InputSlot = 0;
//		element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		element.AlignedByteOffset = shader_parameter.byte_offset;
		element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		element.InstanceDataStepRate = 0;
	}

	DXGI_FORMAT ShaderInterfaceD3D11::GetVertexAttributeFormat(VariableType type)
	{
		DXGI_FORMAT attribute_type = DXGI_FORMAT_UNKNOWN;
		switch (type)
		{
		case kFloat:
			attribute_type = DXGI_FORMAT_R32_FLOAT;
			break;
		case kVector2:
			attribute_type = DXGI_FORMAT_R32G32_FLOAT;
			break;
		case kVector3:
			attribute_type = DXGI_FORMAT_R32G32B32_FLOAT;
			break;
		case kVector4:
			attribute_type = DXGI_FORMAT_R32G32B32A32_FLOAT;
			break;
		case kUByte4:
			attribute_type = DXGI_FORMAT_R32_UINT;
			break;
		}

		return attribute_type;
	}

	void ShaderInterfaceD3D11::CreateVertexShaderConstantBuffer()
	{
		if (vertex_shader_variable_data_size_ > 0)
		{
			// Setup the description of the dynamic constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC constant_buffer_desc;
			constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			constant_buffer_desc.ByteWidth = vertex_shader_variable_data_size_;
			constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constant_buffer_desc.MiscFlags = 0;
			constant_buffer_desc.StructureByteStride = 0;

			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT hresult = S_OK;
			hresult = device_->CreateBuffer(&constant_buffer_desc, NULL, &this->vs_constant_buffer_);
		}

	}

	void ShaderInterfaceD3D11::CreatePixelShaderConstantBuffer()
	{
		if (pixel_shader_variable_data_size_ > 0)
		{
			// Setup the description of the dynamic constant buffer that is in the vertex shader.
			D3D11_BUFFER_DESC constant_buffer_desc;
			constant_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
			constant_buffer_desc.ByteWidth = pixel_shader_variable_data_size_;
			constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constant_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constant_buffer_desc.MiscFlags = 0;
			constant_buffer_desc.StructureByteStride = 0;

			// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
			HRESULT hresult = S_OK;
			hresult = device_->CreateBuffer(&constant_buffer_desc, NULL, &this->ps_constant_buffer_);
		}
	}

	void ShaderInterfaceD3D11::CreateSamplerStates()
	{
		for (auto texture_sampler = texture_samplers_.begin(); texture_sampler != texture_samplers_.end(); ++texture_sampler)
		{
			D3D11_SAMPLER_DESC sampler_desc;

			// Create a texture sampler state description.
			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//			sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
			//sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			//sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			//sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampler_desc.MipLODBias = 0.0f;
			sampler_desc.MaxAnisotropy = 1;
			sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
			sampler_desc.BorderColor[0] = 0;
			sampler_desc.BorderColor[1] = 0;
			sampler_desc.BorderColor[2] = 0;
			sampler_desc.BorderColor[3] = 0;
			sampler_desc.MinLOD = 0;
			sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

			// Create the texture sampler state.
			ID3D11SamplerState* sampler_state;
			HRESULT hresult = device_->CreateSamplerState(&sampler_desc, &sampler_state);
			sampler_states_.push_back(sampler_state);
		}
	}

	void ShaderInterfaceD3D11::AddSamplerState(const D3D11_SAMPLER_DESC& sampler_desc)
	{
		// Create the texture sampler state.
		ID3D11SamplerState* sampler_state;
		HRESULT hresult = device_->CreateSamplerState(&sampler_desc, &sampler_state);
		sampler_states_.push_back(sampler_state);
	}
}