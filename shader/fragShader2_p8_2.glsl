#version 430
in vec3 varyingNormal, varyingLightDir, varyingVertPos, varyingHalfVec;
in vec4 shadow_coord;
out vec4 fragColor;

struct PositionalLight {
	vec4 ambient, diffuse, specular;
	vec3 position;
};
struct Material {
	vec4 ambient, diffuse, specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;
uniform mat4 shadowMVP2;
layout(binding=0) uniform sampler2DShadow shadowTex;

// 从shadow_coord返回距离(x, y)处的纹元的阴影深度值
// shadow_coord是阴影贴图中与正在渲染的当前像素相对应的位置
float lookup(float ox, float oy) {
	float t =textureProj(shadowTex, shadow_coord + vec4(ox * 0.001 * shadow_coord.w, oy * 0.001 * shadow_coord.w, -0.01, 0.0)); // 第三个参数-0.01是用于消除阴影“痤疮”的偏移量
	return t;
}

void main(void) {
	float shadowFactor=0.0;
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);
	vec3 H = normalize(varyingHalfVec);

	// 生成一个4采样抖动的柔和阴影
//	float swidth = 2.5; // 可调整阴影的扩散量
//	// 根据glFragCoord mod 2 生成4中采样模式的一种
//	vec2 offset = mod(floor(gl_FragCoord.xy), 2.0) * swidth;
//	shadowFactor += lookup(-1.5*swidth + offset.x, 1.5*swidth - offset.y);
//	shadowFactor += lookup(-1.5*swidth + offset.x, -0.5*swidth - offset.y);
//	shadowFactor += lookup(0.5*swidth + offset.x, 1.5*swidth - offset.y);
//	shadowFactor += lookup(0.5*swidth + offset.x, -0.5*swidth - offset.y);
//	shadowFactor = shadowFactor / 4.0;  // shadowFactor 是4个采样点的均值

	// ---- 这个是64采样点的
	float swidth = 2.5;
	float endp = swidth * 3.0 + swidth / 2.0;
	for(float m=-endp; m<=endp; m=m+swidth) {
		for (float n=-endp; n<=endp; n=n+swidth) {
			shadowFactor += lookup(m, n);
		}
	}
	shadowFactor = shadowFactor / 64.0;
	vec4 shadowColor = globalAmbient * material.ambient + light.ambient * material.ambient;
	vec4 lightedColor = light.diffuse * material.diffuse * max(dot(L, N), 0.0) + light.specular * material.specular * pow(max(dot(H, N), 0.0), material.shininess * 3.0);
	fragColor = vec4((shadowColor.xyz + shadowFactor*(lightedColor.xyz)), 1.0);
}