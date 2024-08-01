#version 430

layout(location=0) in vec3 position;

uniform mat4 v_matrix;
uniform mat4 proj_matrix;
uniform float tf; // 时间因子

// use to interpolate color
out vec4 varyingColor;
// glsl要求先声明函数，后调用
mat4 buildRotateX(float rad);
mat4 buildRotateY(float rad);
mat4 buildRotateZ(float rad);
mat4 buildTranslate(float x, float y, float z);

void main(void) {
	// 取值基于时间因子，每个立方体实例都不同
	float i = gl_InstanceID + tf;
	// 用来平移x,y,z的分量
	float a = sin(.35 * i) * 8.0;
	float b = sin(.52 * i) * 8.0;
	float c = sin(.70 * i) * 8.0;
	// 构建旋转和平移矩阵
	mat4 localRotX = buildRotateX(1.75 * i);
	mat4 localRotY = buildRotateY(1.75 * i);
	mat4 localRotZ = buildRotateZ(1.75 * i);
	mat4 localTrans = buildTranslate(a, b, c);

	// 构建模型矩阵,然后得出MV矩阵
	mat4 newM_matrix = localTrans * localRotX * localRotY * localRotZ;
	mat4 mv_matrix = v_matrix * newM_matrix;

	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.0);
	// [-1, 1] -> [0, 1]
	varyingColor = vec4(position, 1.0) * 0.5 + vec4(0.5, 0.5, 0.5, 0.5);
}

mat4 buildTranslate(float x, float y, float z)
{	mat4 trans = mat4(	1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		x, y, z, 1.0 );
	return trans;
}

//  rotation around the X axis
mat4 buildRotateX(float rad)
{	mat4 xrot = mat4(	1.0, 0.0, 0.0, 0.0,
		0.0, cos(rad), -sin(rad), 0.0,
		0.0, sin(rad), cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return xrot;
}

//  rotation around the Y axis
mat4 buildRotateY(float rad)
{	mat4 yrot = mat4(	cos(rad), 0.0, sin(rad), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(rad), 0.0, cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return yrot;
}

//  rotation around the Z axis
mat4 buildRotateZ(float rad)
{	mat4 zrot = mat4(	cos(rad), sin(rad), 0.0, 0.0,
		-sin(rad), cos(rad), 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return zrot;
}