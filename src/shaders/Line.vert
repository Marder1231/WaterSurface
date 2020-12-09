#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;

out vec3 v_color;
out vec3 v_normal;
out vec3 v_pos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
void main()
{
	v_color = aColor;
	v_pos = vec3(u_model * vec4(aPos, 1));
    v_normal = mat3(transpose(inverse(u_model))) * aNormal;  

	gl_Position = u_projection * u_view * u_model * vec4(aPos, 1);
}