#version 330 core

// 给光源数据一个结构体
struct Light{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	vec3 position;

    // 光源衰减系数的三个参数
    float constant; // 常数项
    float linear;	// 一次项
    float quadratic;// 二次项

};
// 给物体材质数据一个结构体
struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;

	float shininess;
};

in vec3 position;
in vec3 normal;
in vec2 texCoord;

// 相机坐标
uniform vec3 eye_position;
// 光源
uniform Light light;
// 物体材质
uniform Material material;

uniform int isShadow;

// 纹理数据
uniform sampler2D texture1;

out vec4 fColor;

void main()
{
	if (isShadow == 1) {
		fColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		fColor = texture( texture1, texCoord );

		vec3 norm = (vec4(normal, 0.0)).xyz;

		// 计算四个归一化的向量 N,V,L,R(或半角向量H)
		vec3 N = normalize(norm); // 法向量
		vec3 V = normalize(eye_position - position); // 观察向量，从表面指向眼睛
		vec3 L = normalize(light.position - position); // 光源向量，从表面指向光源
		vec3 R = reflect(-L, N); // 反射向量，从表面反射到观察方向

		// 环境光分量I_a
		vec4 I_a = light.ambient * material.ambient;

		// Task2 计算系数和漫反射分量I_d
		float diffuse_dot = max(dot(N, L), 0.0);
		vec4 I_d = diffuse_dot * light.diffuse * material.diffuse;

		// Task2 计算系数和镜面反射分量I_s
		float specular_dot_pow = pow(max(dot(R, V), 0.0), material.shininess);
		vec4 I_s = specular_dot_pow * light.specular * material.specular;	


		// Task2 计算高光系数beta和镜面反射分量I_s
		// 注意如果光源在背面则去除高光
		if( dot(L, N) < 0.0 ) {
		    I_s = vec4(0.0, 0.0, 0.0, 1.0);
		} 

		// 合并三个分量的颜色，修正透明度
		fColor += I_a + I_d + I_s;
		fColor.a = 1.0;	
	}
}

