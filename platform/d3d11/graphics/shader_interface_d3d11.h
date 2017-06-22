#ifndef _GRAPHICS_D3D11_SHADER_INTERFACE_D3D11_H
#define _GRAPHICS_D3D11_SHADER_INTERFACE_D3D11_H

#include <graphics/shader_interface.h>
#include <d3d11.h>

namespace gef
{
	class ShaderInterfaceD3D11 : public ShaderInterface
	{
	public:
		ShaderInterfaceD3D11(ID3D11Device* device, ID3D11DeviceContext* device_context);
		~ShaderInterfaceD3D11();

		bool CreateProgram();
		void CreateVertexFormat();

		void UseProgram();

		void SetVariableData();
		void SetVertexFormat();
		void ClearVertexFormat();

		void BindTextureResources(const Platform& platform) const;
		void UnbindTextureResources(const Platform& platform) const;

		void AddSamplerState(const D3D11_SAMPLER_DESC& sampler_desc);

	protected:
		void SetInputAssemblyElement(const ShaderParameter& shader_parameter, D3D11_INPUT_ELEMENT_DESC& element);
		void CreateVertexShaderConstantBuffer();
		void CreatePixelShaderConstantBuffer();
		void CreateSamplerStates();

		DXGI_FORMAT GetVertexAttributeFormat(VariableType type);

		ID3D11Device* device_;
		ID3D11DeviceContext* device_context_;
		ID3D11InputLayout* vertex_input_layout_;
		ID3D11VertexShader* vertex_shader_;
		ID3D11PixelShader* pixel_shader_;
		D3D11_INPUT_ELEMENT_DESC* elements_;
		Int32 num_elements_;
		ID3D11Buffer* vs_constant_buffer_;
		ID3D11Buffer* ps_constant_buffer_;
		std::vector<ID3D11SamplerState*> sampler_states_;

	};
}

#endif // _GRAPHICS_D3D11_SHADER_INTERFACE_D3D11_H