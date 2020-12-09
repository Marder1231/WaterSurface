#version 450 core
out vec4 FragColor;

in vec3 v_color;
in vec3 v_pos;
in vec3 v_normal;

void main()
{
	FragColor = vec4(v_color, 1);
}