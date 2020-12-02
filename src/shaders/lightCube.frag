#version 330 core
out vec4 FragColor;

uniform vec3 u_color;

void main()
{
    FragColor = vec4(u_color, 0); // set alle 4 vector values to 1.0
}