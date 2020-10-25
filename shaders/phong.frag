#version 330

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
};

out vec4 outColor;

in vec3 normal;
in vec3 fragPos;

uniform vec3 eyePos;
uniform DirectionalLight dirLight;

void main() {
	vec3 reflectDir = reflect(dirLight.direction, normalize(normal));
	vec3 eyeDir = normalize(eyePos - fragPos);
	vec3 ambient = 1.0f * dirLight.ambient;
	vec3 diffuse = 0.8f * max(0, dot(-dirLight.direction, normal)) * dirLight.diffuse;
	vec3 specular = 0.5f * pow(max(0, dot(reflectDir, eyeDir)), 16) * dirLight.diffuse;
	
	vec3 fragColor = (ambient + diffuse + specular) * vec3(0.5f, 0.5f, 0.5f);
	outColor = vec4(fragColor, 1.0);
}