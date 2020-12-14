#version 450 core
layout(vertices = 3) out;

in vec3 c_position[];
in vec2 c_textCoords[];

out vec3 e_position[];
out vec2 e_textCoords[];

//struct RippleAttribute
//{
//	vec3 GenerateCenter;
//	int AnimateTime;
//};
//in int v_rippleSize[];
//in RippleAttribute v_majorRipples[][];
//out int c_rippleSize[];
//out RippleAttribute c_majorRipples[][];

void main(void)
{
	gl_TessLevelInner[0] = 24;
	gl_TessLevelOuter[0] = 24;
	gl_TessLevelOuter[1] = 24;
	gl_TessLevelOuter[2] = 24;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	e_position[gl_InvocationID] = c_position[gl_InvocationID];
	e_textCoords[gl_InvocationID] = c_textCoords[gl_InvocationID];

//	c_rippleSize[0] = v_rippleSize[0];
//	for(int i = 0 ; i < c_rippleSize[0]; i++)
//	{
//		c_majorRipples[gl_InvocationID][i] = v_majorRipples[gl_InvocationID][i];
//	}
}