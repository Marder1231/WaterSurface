#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 v_texCoords;

uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	v_texCoords = aTexCoords;
	gl_Position = u_projection * u_view * vec4(aPos, 1);
}