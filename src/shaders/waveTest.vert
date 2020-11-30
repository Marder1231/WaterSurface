#version 450 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 aTexture;


uniform float time; /* in milliseconds */
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 t = vPosition;
    t.y = 0.1*sin(0.001*time+5.0*vPosition.x)*sin(0.001*time+5.0*vPosition.z);
    gl_Position =  projection * view * t;
}