#define NUM_LIGHTS 4

cbuffer LightBuffer
{
   float4 material_colour;
   float4 ambient_light_colour;
   float4 light_colour[NUM_LIGHTS];
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
    float4 lightViewPosition : TEXCOORD5; 
};

Texture2D diffuse_texture;
Texture2D depth_texture;

SamplerState SamplerClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;
};

SamplerState SamplerWrap
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS( PixelInput input ) : SV_Target
{
	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.001f;
    
    float diffuse_light1 = 0.0;
    float diffuse_light2 = 0.0;
    float diffuse_light3 = 0.0;
    float diffuse_light4 = 0.0; 

	// Calculate the projected texture coordinates.
	float2 projectTexCoord;
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		float depthValue = depth_texture.Sample(SamplerClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		float lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if(lightDepthValue < depthValue)
		{
		    // Calculate the amount of light on this pixel.

            
            diffuse_light1 = saturate(dot(input.normal, input.light_vector1));
            diffuse_light2 = saturate(dot(input.normal, input.light_vector2));
            diffuse_light3 = saturate(dot(input.normal, input.light_vector3));
            diffuse_light4 = saturate(dot(input.normal, input.light_vector4));            
		}
	}
    else
    {
            diffuse_light1 = saturate(dot(input.normal, input.light_vector1));
            diffuse_light2 = saturate(dot(input.normal, input.light_vector2));
            diffuse_light3 = saturate(dot(input.normal, input.light_vector3));
            diffuse_light4 = saturate(dot(input.normal, input.light_vector4));            
    }



    float4 diffuse_texture_colour = diffuse_texture.Sample( SamplerWrap, input.uv );
    float4 diffuse_colour1 = diffuse_light1*light_colour[0];
    float4 diffuse_colour2 = diffuse_light2*light_colour[1];
    float4 diffuse_colour3 = diffuse_light3*light_colour[2];
    float4 diffuse_colour4 = diffuse_light4*light_colour[3];
    return saturate(ambient_light_colour+diffuse_colour1+diffuse_colour2+diffuse_colour3+diffuse_colour4)*diffuse_texture_colour*material_colour;
}
