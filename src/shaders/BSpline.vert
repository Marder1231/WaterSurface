#version 450 core
layout(location = 0) in vec3 aPos;

void main()
{
//    v_normal = mat3(transpose(inverse(u_model))) * aNormal;  

	gl_Position = vec4(aPos, 1);
}