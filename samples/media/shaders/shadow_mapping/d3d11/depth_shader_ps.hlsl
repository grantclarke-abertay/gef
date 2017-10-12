struct PixelInput
{
    float4 position : SV_POSITION;
    float4 depth_position : TEXTURE0;
};

float4 PS( PixelInput input ) : SV_Target
{
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	float depth_value = input.depth_position.z / input.depth_position.w;
    float4 colour = float4(depth_value, depth_value, depth_value, 1);

	return colour;
}
