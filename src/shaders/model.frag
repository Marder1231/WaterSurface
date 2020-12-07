#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

float uniformQuantization(float f, int step)
{
    f*=256;
    float q = 256.0 / step;
    int interval = int(f / q);

    f = interval * q;

    return f / 256.0;
}
vec3 DitherCluster(vec3 originColor)
{
    float newPixel[3] = {
        uniformQuantization(originColor.x, 8),
        uniformQuantization(originColor.y, 8),
        uniformQuantization(originColor.z, 4),
    };

    return vec3(newPixel[0], newPixel[1], newPixel[2]);
}

void main()
{    
    FragColor = vec4(DitherCluster(vec3(texture(texture_diffuse1, TexCoords))), 1);
}