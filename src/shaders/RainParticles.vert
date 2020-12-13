#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aDirection;

out Vs_Out
{
	vec3 v_dir;
} vs_out;

void main()
{
	vs_out.v_dir = aDirection;
	gl_Position = vec4(aPos, 1);
}