#define MAX_LIGHTS 4

attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;

uniform mat4 wvp;
uniform mat4 world;
uniform mat4 invworld;
uniform vec4 light_position[MAX_LIGHTS];


varying vec2 ps_in_uv;
varying vec3 ps_in_normal;
varying vec3 ps_in_light_vector1;
varying vec3 ps_in_light_vector2;
varying vec3 ps_in_light_vector3;
varying vec3 ps_in_light_vector4;

void main(void)
{
    gl_Position = vec4(position,1)* wvp;
    ps_in_uv = uv;

    vec4 world_normal = vec4(normal, 0);
    world_normal = world_normal * invworld;
    ps_in_normal = normalize(world_normal.xyz);

    vec4 world_position = vec4(position, 1) * world;
    ps_in_light_vector1 = light_position[0].xyz - world_position.xyz;
    ps_in_light_vector1 = normalize(ps_in_light_vector1);
    ps_in_light_vector2 = light_position[1].xyz - world_position.xyz;
    ps_in_light_vector2 = normalize(ps_in_light_vector2);
    ps_in_light_vector3 = light_position[2].xyz - world_position.xyz;
    ps_in_light_vector3 = normalize(ps_in_light_vector3);
    ps_in_light_vector4 = light_position[3].xyz - world_position.xyz;
    ps_in_light_vector4 = normalize(ps_in_light_vector4);
}

