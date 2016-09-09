uniform sampler2D texture_sampler;
varying vec2 texCoord;
void main (void)
{
	vec4 textureColour = texture2D(texture_sampler, texCoord);
	gl_FragColor = textureColour;
}

