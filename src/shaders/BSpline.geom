#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 246) out;

struct ControlPointAttribute
{
	vec3 position;
	vec3 orient;
};

#define MAX_CONTROL_POINT 100
uniform int u_ControlPointAmount;
uniform ControlPointAttribute cp[MAX_CONTROL_POINT];
uniform int u_nowControlPointIndex;
uniform float u_clipSize = float(24);

uniform mat4 u_view;
uniform mat4 u_projection;

vec3 GetPos_BSpline(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4)
{
	float interval = t - int(t);
	
	mat4 matM = mat4
	(
		-1 / 6.0f, 3 / 6.0f, -3 / 6.0f, 1 / 6.0f,
		3 / 6.0f, -6 / 6.0f, 3 / 6.0f, 0,
		-3 / 6.0f, 0, 3 / 6.0f, 0,
		1 / 6.0f, 4 / 6.0f, 1 / 6.0f, 0
	);

	vec4 vecT = vec4
	(
		interval * interval  * interval, 
		interval * interval, 
		interval, 
		1
	);

	mat4x3 matP = mat4x3
	(
		p1.x, p1.y, p1.z,
		p2.x, p2.y, p2.z,
		p3.x, p3.y, p3.z,
		p4.x, p4.y, p4.z
	);

	vec3 qt = matP * matM * vecT;

	return qt;
}

void main()
{
	float t = u_nowControlPointIndex;
	vec3 cpS = GetPos_BSpline(t, 
		cp[int(t + 0) % u_ControlPointAmount].position,
		cp[int(t + 1) % u_ControlPointAmount].position,
		cp[int(t + 2) % u_ControlPointAmount].position,
		cp[int(t + 3) % u_ControlPointAmount].position
	);
	vec3 cpSo = GetPos_BSpline(t, 
		cp[int(t + 0) % u_ControlPointAmount].orient,
		cp[int(t + 1) % u_ControlPointAmount].orient,
		cp[int(t + 2) % u_ControlPointAmount].orient,
		cp[int(t + 3) % u_ControlPointAmount].orient
	);
	gl_Position = u_projection * u_view * vec4(cpS, 1);
	EmitVertex();

	float percent = 1.0f / u_clipSize;

	for(int j = 0; j < u_clipSize; j++)
	{
		t += percent;
		
		vec3 cpN = GetPos_BSpline(t, 
			cp[int(t + 0) % u_ControlPointAmount].position,
			cp[int(t + 1) % u_ControlPointAmount].position,
			cp[int(t + 2) % u_ControlPointAmount].position,
			cp[int(t + 3) % u_ControlPointAmount].position
		);
		vec3 cpNo = GetPos_BSpline(t, 
			cp[int(t + 0) % u_ControlPointAmount].orient,
			cp[int(t + 1) % u_ControlPointAmount].orient,
			cp[int(t + 2) % u_ControlPointAmount].orient,
			cp[int(t + 3) % u_ControlPointAmount].orient
		);



		gl_Position = u_projection * u_view * vec4(cpN, 1);
		EmitVertex();

		cpN = cpS;
	}

	EndPrimitive();
}