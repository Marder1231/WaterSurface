#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexture;

struct ControlPointAttribute
{
	vec3 position;
	vec3 orient;
};
//compute choice control points
uniform ControlPointAttribute cp[4];
uniform float u_time;

uniform float u_aSegment;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 v_texture;

vec3 GetPos_BSpline(float t, vec3 p1, vec3 p2, vec3 p3, vec3 p4)
{
	float interval = t - int(t);
	
	mat4 matM = mat4
	(
		-0.5f, 1.5f, -1.5f, 0.5f, 
			1, -2.5f, 2, -0.5f, 
			-0.5f, 0, 0.5f, 0, 
			0, 1, 0, 0
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
	vec3 cpS = GetPos_BSpline(u_time,
		cp[0].position,
		cp[1].position,
		cp[2].position,
		cp[3].position
	);
	vec3 cpN = GetPos_BSpline(u_time - u_aSegment / 2,
		cp[0].position,
		cp[1].position,
		cp[2].position,
		cp[3].position
	);

	vec3 cpSo = GetPos_BSpline(u_time,
		cp[0].orient,
		cp[1].orient,
		cp[2].orient,
		cp[3].orient
	);
	vec3 cpNo = GetPos_BSpline(u_time - u_aSegment / 2,
		cp[0].orient,
		cp[1].orient,
		cp[2].orient,
		cp[3].orient
	);

	vec3 u = normalize(cpN - cpS);
	vec3 w = normalize(u * cpNo);
	vec3 v = normalize(w * u);
	
	mat4 rotation = mat4(1.0f);
	rotation[0][0] = u.x;	rotation[1][0] = v.x;	rotation[2][0] = w.x;		rotation[3][0] = 0;
	rotation[0][1] = u.y;	rotation[1][1] = v.y;	rotation[2][1] = w.y;		rotation[3][1] = 0;
	rotation[0][2] = u.z;	rotation[1][2] = v.z;	rotation[2][2] = w.z;		rotation[3][2] = 0;
	rotation[0][3] = 0;		rotation[1][3] = 0;		rotation[2][3] = 0;			rotation[3][3] = 1;

	mat4 translate = mat4(1.0f);
	translate[0][0] = 1;	translate[1][0] = 0;	translate[2][0] = 0;		translate[3][0] = cpS.x;
	translate[0][1] = 0;	translate[1][1] = 1;	translate[2][1] = 0;		translate[3][1] = cpS.y;
	translate[0][2] = 0;	translate[1][2] = 0;	translate[2][2] = 1;		translate[3][2] = cpS.z;
	translate[0][3] = 0;	translate[1][3] = 0;	translate[2][3] = 0;		translate[3][3] = 1;
	
	mat4 model = mat4(1.0f);
	model =  translate ;

	v_texture = aTexture;
	gl_Position = u_projection * u_view  * translate * vec4(aPos, 1);
}