#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
uniform float vx_offset = 0.65f;
uniform float pixel_w = 15.0f;
uniform float pixel_h = 10.0f;
uniform float rt_w = 590; // GeeXLab built-in
uniform float rt_h = 590; // GeeXLab built-in
uniform sampler2D u_screenTexture;

uniform float u_time;
void main()
{
//------------------Pixelation---------------------------------
    vec2 uv = TexCoords.xy;
    vec3 tc = vec3(1, 0, 0);

    if(uv.x < (vx_offset - 0.005f))
    {
        float dx = pixel_w * (1.0f / rt_w);
        float dy = pixel_h * (1.0f / rt_h);
        vec2 coord = vec2(dx * floor(uv.x / dx), dy * floor(uv.y / dy));
        tc = texture2D(u_screenTexture, coord).rgb;
    }
    else if( uv.x >= (vx_offset + 0.005f))
    {
        tc = texture2D(u_screenTexture, uv).rgb;
    }

    FragColor = vec4(tc, 1.0f);

//----------------OffSet----------------------------------
    FragColor = vec4(texture( u_screenTexture, uv + 0.005f * vec2 ( sin( u_time + 1024.0f * uv.x), cos(u_time + 768.0f * uv.y)) ).rgb, 1);


//-----------------------Others------------------------------
    vec3 col = texture(u_screenTexture, TexCoords).rgb;

    float amplitude = 0.0f;
    col.x += amplitude;  col.x = clamp(col.x, 0, 1);
    col.y += amplitude;  col.y = clamp(col.y, 0, 1);
    col.z += amplitude;  col.z = clamp(col.z, 0, 1);

    FragColor = vec4(vec3(col), 1.0);
} 