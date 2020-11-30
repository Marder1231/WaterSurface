#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D heightMap;
void main()
{
	float amplitude = 100;
	float speed = 2;
	float waveLength = 10;

    vec3 p = vec3(model * vec4(aPos, 1));
	float height = vec3(normalize(texture(heightMap, aTextCoords))).x;
	height = (height - 0.5f) * amplitude;
	p.y += height;
    FragPos = p;

	gl_Position = projection * view * vec4(FragPos, 1.0f);
}

/*
0.0f,  1.0f,
1.0f,  1.0f,
1.0f,  0.0f,
1.0f,  0.0f,
0.0f,  0.0f,
0.0f,  1.0f
*/