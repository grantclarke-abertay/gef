#define NUM_LIGHTS 4
#define NUM_MATRICES 128


#define MAX_LIGHTS 4

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 wvp;
uniform mat4 world;
uniform vec4 light_position[MAX_LIGHTS];
uniform mat4 bone_matrices[NUM_MATRICES];


varying vec2 ps_in_uv;
varying vec3 ps_in_normal;
varying vec3 ps_in_light_vector1;
varying vec3 ps_in_light_vector2;
varying vec3 ps_in_light_vector3;
varying vec3 ps_in_light_vector4;

void VS( in VertexInput input,
         out PixelInput output )

void main(void)
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
