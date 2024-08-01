#version 430
in vec3 varyingNormal; //视觉空间顶点法向量
in vec3 varyingLightDir; // 指向光源的向量
in vec3 varyingVertPos; // 视觉空间中的顶点位置
out vec4 fragColor;

// 与顶点着色器相同的同一变量
// 但并不直接在当前片段着色器使用

struct PositionalLight {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};
struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};
uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix; // 用来变换法向量

void main(void) {
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);

	// 计算光照向量基于N的反射向量
	vec3 R = normalize(reflect(-L, N));
	// 计算光照与平面法向量间的角度
	float cosTheta = dot(L, N);
	// 计算视觉向量与反射光向量的角度
	float cosPhi = dot(V, R);

	// 计算ADS分量（按像素），并合并以构建输出颜色
	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
	vec3 specular = material.specular.xyz * light.specular.xyz * pow(max(cosPhi, 0.0f), material.shininess);
	fragColor = vec4(ambient + diffuse + specular, 1.0);
}