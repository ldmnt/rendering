#version 330

struct DirectionalLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec3 fragColor;

layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 eyePos;
uniform DirectionalLight dirLight;

void main() {
	vec3 reflectDir = reflect(dirLight.direction, inNormal);
	vec3 eyeDir = normalize(eyePos - inVertex);
	vec3 ambient = 1.0f * dirLight.ambient;
	vec3 diffuse = 0.8f * max(0, dot(-dirLight.direction, inNormal)) * dirLight.diffuse;
	vec3 specular = 0.5f * pow(max(0, dot(reflectDir, eyeDir)), 16) * dirLight.diffuse;
	
	fragColor = (ambient + diffuse + specular) * vec3(0.5f, 0.5f, 0.5f);
	gl_Position = projection * view * transform * vec4(inVertex, 1.0f);
}