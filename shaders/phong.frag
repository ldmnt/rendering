#version 330

in vec3 normal;
in vec3 lightDir;
in float ambientLight;

out vec4 outColor;

void main() {
	vec3 fragColor = (max(0, dot(normalize(-lightDir), normalize(normal))) + ambientLight) * vec3(0.8f, 0.6f, 0.2f);
	outColor = vec4(fragColor, 1.0);
}