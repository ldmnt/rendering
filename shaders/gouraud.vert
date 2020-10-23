#version 330

layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;

out vec3 fragColor;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 inLightDir;
uniform float inAmbientLight;

void main() {
	fragColor = (max(0, dot(normalize(-inLightDir), normalize(inNormal))) + inAmbientLight) * vec3(0.8f, 0.6f, 0.2f);
	gl_Position = projection * view * transform * vec4(inVertex, 1.0f);
}