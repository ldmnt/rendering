#version 330

layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;

out vec3 normal;
out vec3 lightDir;
out float ambientLight;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 inLightDir;
uniform float inAmbientLight;

void main() {
	normal = inNormal;
	lightDir = inLightDir;
	ambientLight = inAmbientLight;
	gl_Position = projection * view * transform * vec4(inVertex, 1.0f);
}