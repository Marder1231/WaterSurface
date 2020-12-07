#version 450 core

out vec4 FragColor;

uniform sampler1D tex_toon;
uniform vec3  lightPos;

in VS_OUT
{
	vec3 normal;
	vec3 view;
}v_out;



void main()
{
	vec3 N = normalize(v_out.normal);
	vec3 L = normalize(lightPos - v_out.view);

	float tc = pow(max(0.0f, dot(N, L)), 5.0f);
	FragColor =  texture(tex_toon, tc) * (tc * 0.8f + 0.2f);
}