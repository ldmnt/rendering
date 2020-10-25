#version 330

layout (location = 0) in vec3 inVertex;
layout (location = 1) in vec3 inNormal;

out vec3 normal;
out vec3 fragPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fragPos = vec3(transform * vec4(inVertex, 1.0));
	gl_Position = projection * view * transform * vec4(inVertex, 1.0f);
	normal = inNormal;
}