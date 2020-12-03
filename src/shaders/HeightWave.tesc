#version 450 core
layout(vertices = 3) out;

in vec3 c_position[];
in vec2 c_textCoords[];

out vec3 e_position[];
out vec2 e_textCoords[];

void main(void)
{
	gl_TessLevelInner[0] = 2460;
	gl_TessLevelOuter[0] = 2460;
	gl_TessLevelOuter[1] = 2460;
	gl_TessLevelOuter[2] = 2460;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	e_position[gl_InvocationID] = c_position[gl_InvocationID];
	e_textCoords[gl_InvocationID] = c_textCoords[gl_InvocationID];
}