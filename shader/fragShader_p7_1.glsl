#version 430
in vec4 varyingColor;
out vec4 fragColor;

// �붥����ɫ����ͬ��ͬһ����
// ������ֱ���ڵ�ǰƬ����ɫ��ʹ��

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
uniform mat4 norm_matrix; // �����任������

void main(void) {
	fragColor = varyingColor;
}