#version 450 core
out vec4 FragColor;

in vec2 v_texture;

uniform sampler2D u_roadTexture;

void main()
{
	FragColor = texture(u_roadTexture, v_texture);	
	FragColor = vec4(0, 0, 0, 1);
}