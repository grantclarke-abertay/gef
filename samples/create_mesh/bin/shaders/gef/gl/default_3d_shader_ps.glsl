#define NUM_LIGHTS 4

varying vec2 ps_in_uv;
varying vec3 ps_in_normal;
varying vec3 ps_in_light_vector1;
varying vec3 ps_in_light_vector2;
varying vec3 ps_in_light_vector3;
varying vec3 ps_in_light_vector4;

uniform vec4 material_colour;
uniform vec4 ambient_light_colour;
uniform vec4 light_colour[NUM_LIGHTS];
uniform sampler2D texture_sampler;

void main (void)
{
    float diffuse_light1 = clamp(dot(ps_in_normal, ps_in_light_vector1), 0.0, 1.0);
    float diffuse_light2 = clamp(dot(ps_in_normal, ps_in_light_vector2), 0.0, 1.0);
    float diffuse_light3 = clamp(dot(ps_in_normal, ps_in_light_vector3), 0.0, 1.0);
    float diffuse_light4 = clamp(dot(ps_in_normal, ps_in_light_vector4), 0.0, 1.0);

    vec3 diffuse_colour1 = diffuse_light1*light_colour[0].xyz;
    vec3 diffuse_colour2 = diffuse_light2*light_colour[1].xyz;
    vec3 diffuse_colour3 = diffuse_light3*light_colour[2].xyz;
    vec3 diffuse_colour4 = diffuse_light4*light_colour[3].xyz;

    vec4 diffuse_colour = vec4(diffuse_colour1+diffuse_colour2+diffuse_colour3+diffuse_colour4, 1);

    vec4 diffuse_texture_colour = texture2D(texture_sampler, ps_in_uv);
    gl_FragColor = clamp(ambient_light_colour+diffuse_colour, vec4(0.0, 0.0, 0.0, 1.0),  vec4(1.0, 1.0, 1.0, 1.0))*diffuse_texture_colour*material_colour;
}

