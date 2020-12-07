#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;

out VS_OUT
{
	vec3 normal;
	vec3 view;
}v_out;

void main()
{
	vec4 pos_vs = u_view  * u_model * vec4(aPos, 1);

	v_out.normal = mat3(u_view * u_model) * aNormal;
	v_out.view = pos_vs.xyz;
	gl_Position = u_projection * pos_vs;
}