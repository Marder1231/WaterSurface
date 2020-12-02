#version 450 core
layout (location = 0) in vec3 aPos;

uniform vec4 u_model;
uniform vec4 u_view;
uniform vec4 u_projection;
void main()
{
	vec4 pos = u_model * vec4(aPos, 1);

	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1);
}