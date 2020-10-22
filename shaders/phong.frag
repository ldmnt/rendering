#version 330

in vec3 inNormal;
in vec3 inLightDir;
in float inAmbientLight;

out vec4 outColor;

void main() {
	vec3 fragColor = (max(0, dot(normalize(-inLightDir), normalize(inNormal))) + inAmbientLight) * vec3(0.8f, 0.6f, 0.2f);
	outColor = vec4(fragColor, 1.0);
}