#version 450 core

layout(triangles, equal_spacing, ccw)in;

in vec3 e_position[];
in vec2 e_textCoords[];

out vec3 f_position;
out vec2 f_textCoords;

uniform mat4 u_view;
uniform mat4 u_projection;

uniform sampler2D u_heightMap;

uniform vec3 RippleCenter;
uniform float u_time;

//reference : http://www.zwqxin.com/archives/opengl/water-simulation-3.html
float Ripple(float dist_to_waveCenter, float waveCenterAmplitude, float time)
{
	float attenuate = (1 + 0.07f * time * time + 10.0f * dist_to_waveCenter * dist_to_waveCenter);

	float factor = waveCenterAmplitude / attenuate;

	float origninFreq = 0.05f;

	float freq = origninFreq / (1.0f + 0.07f * time);

	float deltaT = dist_to_waveCenter / freq;

	float DestVertexHeight = factor * cos( (time) + 3.1415926);

	return DestVertexHeight * 0.1f;
}	

float CircleWave(float dist_to_waveCenter, float waveCenterAmplitude, float time)
{
	float attenuate = 24.6f / (time + 1);
	return  attenuate * sin(dist_to_waveCenter - time) / pow(2, abs(dist_to_waveCenter - time))
			- 10 * Ripple(dist_to_waveCenter, waveCenterAmplitude, int(time));
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}
vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}
void main(void)
{
	float amplitude = 2.460f;
	float speed = 2;
	float waveLength = 10;

	f_position = interpolate3D(e_position[0],e_position[1], e_position[2]);
	f_textCoords = interpolate2D(e_textCoords[0],e_textCoords[1], e_textCoords[2]);

	vec3 p = f_position;
	float height = vec3(texture(u_heightMap, f_textCoords)).x;
//    float height = sin(f_textCoords.x*2*2*3.1415926);
	height = (height - 0.5f) * amplitude;

	p.y +=  height + CircleWave(length(RippleCenter - f_position), amplitude, u_time);
	f_position = p;

	gl_Position = u_projection * u_view * vec4(f_position, 1);
}