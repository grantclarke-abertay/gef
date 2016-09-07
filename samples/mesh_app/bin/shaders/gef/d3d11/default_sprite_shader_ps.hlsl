struct PixelInput
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D diffuse_texture;

SamplerState Sampler0
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
float4 PS( PixelInput input ) : SV_Target
{
    float4 diffuse_texture_colour = diffuse_texture.Sample( Sampler0, input.uv )*input.colour;
    return diffuse_texture_colour;
}
