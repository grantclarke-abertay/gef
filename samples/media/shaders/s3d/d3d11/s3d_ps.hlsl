struct PixelInput
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float2 uv : TEXCOORD;
};

Texture2D left_eye_texture;
Texture2D right_eye_texture;

SamplerState LeftEyeSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState RightEyeSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS( PixelInput input ) : SV_Target
{
	float4 left_eye_colour, right_eye_colour, final_colour;
    left_eye_colour = left_eye_texture.Sample( LeftEyeSampler, input.uv );
    right_eye_colour = right_eye_texture.Sample( RightEyeSampler, input.uv );
	final_colour = saturate((left_eye_colour+right_eye_colour)*input.colour);
    return final_colour;
}
