#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec3 c_position;
out vec2 c_textCoords;

uniform mat4 u_model;
void main()
{
	c_position = vec3(u_model * vec4(aPos, 1));
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