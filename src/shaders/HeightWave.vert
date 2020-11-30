#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextCoords;

out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D height;

void main()
{
	float amplitude = 5;
	float speed = 2;
	float waveLength = 10;

    vec3 p = vec3(model * vec4(aPos, 1));

	vec3 heightMap = normalize(vec3(texture(height , aTextCoords)));

	float gray = 0.299 * heightMap.x + 0.587 * heightMap.y + 0.114 * heightMap.z;

	p.y = (gray - 0.5f)*amplitude;
    FragPos = p;

	gl_Position = projection * view * vec4(FragPos, 1.0f);
}
