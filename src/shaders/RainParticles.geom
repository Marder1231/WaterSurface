#version 450 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;


uniform mat4 u_view;
uniform mat4 u_projection;

in Vs_Out
{
	vec3 v_dir;
} vs_out[];

void main()
{
	gl_Position = u_projection * u_view * gl_in[0].gl_Position;
	EmitVertex();

	float lineLength = 2.46f;
	vec4 dirPos = vec4((vec3(gl_in[0].gl_Position) + lineLength * vs_out[0].v_dir), 1);

	gl_Position = u_projection * u_view * dirPos;
	EmitVertex();

	EndPrimitive();
}