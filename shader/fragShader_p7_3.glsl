#version 430
in vec3 varyingNormal; //�Ӿ��ռ䶥�㷨����
in vec3 varyingLightDir; // ָ���Դ������
in vec3 varyingVertPos; // �Ӿ��ռ��еĶ���λ��
out vec4 fragColor;
in vec3 varyingHalfVector;

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
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	//vec3 V = normalize(-varyingVertPos);
	vec3 H = normalize(varyingHalfVector);

	// ���������������N�ķ�������
	vec3 R = normalize(reflect(-L, N));
	// ���������ƽ�淨������ĽǶ�
	float cosTheta = dot(L, N);
	// �����Ӿ������뷴��������ĽǶ�
	//float cosPhi = dot(V, R);
	// ���㷨����N���ƽ��������H֮��ĽǶ�
	float cosPhi = dot(H, N);

	// ����ADS�����������أ������ϲ��Թ��������ɫ
	vec3 ambient = ((globalAmbient * material.ambient) + (light.ambient * material.ambient)).xyz;
	vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
	// ��3.0���ƾ���߹�Ч��
	vec3 specular = material.specular.xyz * light.specular.xyz * pow(max(cosPhi, 0.0f), material.shininess * 3.0);
	fragColor = vec4((ambient + diffuse + specular), 1.0);
}