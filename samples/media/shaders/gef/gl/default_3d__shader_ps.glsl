varying vec2 texCoord;
uniform sampler2D texture_sampler;

void main (void)
{
	vec4 textureColour = texture2D(texture_sampler, texCoord);
	gl_FragColor = textureColour;
}
