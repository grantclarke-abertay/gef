#define NUM_LIGHTS 4

cbuffer MatrixBuffer
{
	matrix wvp;
	matrix world;
	matrix invworld;
   float4 light_position[NUM_LIGHTS];
};

struct VertexInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
//	float4 blendweights : BLENDWEIGHT;
//	int blendindices : BLENDINDICES;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 normal: NORMAL;
    float2 uv : TEXCOORD0;
    float3 light_vector1 : TEXCOORD1;
    float3 light_vector2 : TEXCOORD2;
    float3 light_vector3 : TEXCOORD3;
    float3 light_vector4 : TEXCOORD4;
};

void VS( in VertexInput input,
         out PixelInput output )
{
    output.position = mul(input.position, wvp);
    output.uv = input.uv;

    float4 normal = float4(input.normal, 0);
    normal = mul(normal, invworld);
    output.normal = normalize(normal.xyz);

    float4 world_position = mul(input.position, world);
    output.light_vector1 = light_position[0].xyz - world_position.xyz;
    output.light_vector1 = normalize(output.light_vector1);
    output.light_vector2 = light_position[1].xyz - world_position.xyz;
    output.light_vector2 = normalize(output.light_vector2);
    output.light_vector3 = light_position[2].xyz - world_position.xyz;
    output.light_vector3 = normalize(output.light_vector3);
    output.light_vector4 = light_position[3].xyz - world_position.xyz;
    output.light_vector4 = normalize(output.light_vector4);
}
