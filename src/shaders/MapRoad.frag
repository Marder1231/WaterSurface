#version 450 core
out vec4 FragColor;

in vec2 v_texCoords;

uniform vec3 u_color = vec3(0, 1, 1);

uniform sampler2D u_roadTexture;
void main()
{

	FragColor = texture(u_roadTexture, v_texCoords);
}