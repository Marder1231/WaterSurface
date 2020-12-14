#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec3 c_position;
out vec2 c_textCoords;

#define MAX_RIPPLE_AMOUNT 100
//struct RippleAttribute
//{
//	vec3 GenerateCenter;
//	int AnimateTime;
//};
//uniform int NumOfRipples;
//uniform RippleAttribute Ripples[MAX_RIPPLE_AMOUNT];
//out int v_rippleSize;
//out RippleAttribute v_majorRipples[]; 

uniform mat4 u_model;
void main()
{
	c_position = vec3(u_model * vec4(aPos, 1));
    
//	int counter = -1;
//	for(int i = 0; i < NumOfRipples; i++)
//	{
//		if(length(Ripples[i].GenerateCenter - c_position) < 6)
//		{
//			counter++;
//			v_majorRipples[counter] = Ripples[i];
//		}
//	}
//	v_rippleSize = counter;

	c_textCoords = vec2(aTextCoords.x, 1.0f - aTextCoords.y);
}

/*
0.0f,  1.0f,
1.0f,  1.0f,
1.0f,  0.0f,
1.0f,  0.0f,
0.0f,  0.0f,
0.0f,  1.0f
*/