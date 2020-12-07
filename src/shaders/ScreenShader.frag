#version 450 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_screenTexture;

void main()
{
    vec3 col = texture(u_screenTexture, TexCoords).rgb;
    FragColor = vec4(vec3( col), 1.0);
} 