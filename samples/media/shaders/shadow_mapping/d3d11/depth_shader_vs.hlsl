cbuffer MatrixBuffer
{
	matrix wvp;
};

struct VertexInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 depth_position : TEXTURE0;
};

void VS( in VertexInput input,
         out PixelInput output )
{
    output.position = mul(input.position, wvp);
    output.depth_position = output.position;
}
