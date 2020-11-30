#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform float time;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	float amplitude = .5;
	float speed = 2;
	float waveLength = 10;

    vec3 p = vec3(model * vec4(aPos, 1));
	float k = 2 * 3.1415926 / waveLength;
	float f = k * p.x - speed * time;
	p.y = amplitude * sin(f);
	
	vec3 tangent = normalize(vec3(1, k * amplitude * cos(f), 0));
	vec3 normal = vec3(-tangent.y, tangent.x, 0);

    FragPos = p;
	Normal = normal;
	TexCoords = aTextCoords;

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