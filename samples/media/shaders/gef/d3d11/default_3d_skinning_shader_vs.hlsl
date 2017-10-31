#define NUM_LIGHTS 4
#define NUM_MATRICES 128

cbuffer MatrixBuffer
{
	matrix wvp;
	matrix world;
	matrix invworld;
	float4 light_position[NUM_LIGHTS];
	matrix bone_matrices[NUM_MATRICES];
};

struct VertexInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
	int blendindices : BLENDINDICES;
	float4 blendweights : BLENDWEIGHT;
    float2 uv : TEXCOORD;
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
    output.uv = input.uv;
	
	uint4 indices = uint4(
	(input.blendindices & 0x000000ff),
	(input.blendindices & 0x0000ff00) >> 8,
	(input.blendindices & 0x00ff0000) >> 16,
	(input.blendindices & 0xff000000) >> 24
	);

    float4 normal = float4(input.normal, 0);
	input.position.w = 1.0;
	
	// bone 0
	float4 world_position = input.blendweights.x*mul(input.position, bone_matrices[indices.x]);
	float4 world_normal = input.blendweights.x*mul(normal, bone_matrices[indices.x]);

	// bone 1
	world_position += input.blendweights.y*mul(input.position, bone_matrices[indices.y]);
	world_normal += input.blendweights.y*mul(normal, bone_matrices[indices.y]);
	
	// bone 2
	world_position += input.blendweights.z*mul(input.position, bone_matrices[indices.z]);
	world_normal += input.blendweights.z*mul(normal, bone_matrices[indices.z]);

	// bone 3
	world_position += input.blendweights.w*mul(input.position, bone_matrices[indices.w]);
	world_normal += input.blendweights.w*mul(normal, bone_matrices[indices.w]);

    normal = mul(world_normal, invworld);
    output.normal = normalize(normal.xyz);
	
    output.position = mul(world_position, wvp);	
    world_position = mul(world_position, world);
	
    output.light_vector1 = light_position[0].xyz - world_position.xyz;
    output.light_vector1 = normalize(output.light_vector1);
    output.light_vector2 = light_position[1].xyz - world_position.xyz;
    output.light_vector2 = normalize(output.light_vector2);
    output.light_vector3 = light_position[2].xyz - world_position.xyz;
    output.light_vector3 = normalize(output.light_vector3);
    output.light_vector4 = light_position[3].xyz - world_position.xyz;
    output.light_vector4 = normalize(output.light_vector4);
}
