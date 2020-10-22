#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;

out vec3 inNormal;
out vec3 inLightDir;
out float inAmbientLight;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightDir;
uniform float ambientLight;

void main() {
	inNormal = normal;
	inLightDir = lightDir;
	inAmbientLight = ambientLight;
	gl_Position = projection * view * transform * vec4(vertex, 1.0f);
}