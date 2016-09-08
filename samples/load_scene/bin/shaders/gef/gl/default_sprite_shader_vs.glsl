attribute vec3 position;
uniform mat4 proj_matrix;
uniform mat4 sprite_data;         // input
varying vec2 texCoord;
varying vec4 vertexColor;

void main(void)
{
    vec2 transVertex = vec2(sprite_data[2][0], sprite_data[2][1] ) + mat2( sprite_data[0][0], sprite_data[0][1], sprite_data[1][0], sprite_data[1][1]) * position.xy;
    gl_Position = vec4(transVertex, sprite_data[2][2],1)* proj_matrix;
//    vertexColor = vec4( sprite_data[3][0], sprite_data[3][1], sprite_data[3][2], sprite_data[3][3] );
    texCoord = (position.xy + vec2(0.5, 0.5)) * vec2(sprite_data[1][2], sprite_data[1][3]) + vec2(sprite_data[0][2], sprite_data[0][3]);
}

