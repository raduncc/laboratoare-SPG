#version 430

layout(location = 0) in vec2 texture_coord;

uniform sampler2D texture_1;

layout(location = 0) out vec4 out_color;

void main()
{
	//TODO apply texture
	vec4 color = texture2D(texture_1, texture_coord);
	//TODO discard when alfa component < 0.75
	if (color.a < 0.75){
	discard;
	}
	out_color = color;
}