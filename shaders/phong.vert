#version 330

layout (location = 0) in vec4 inVertex;
layout (location = 1) in vec3 inNormal;

out vec3 normal;
out vec4 fragPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fragPos = transform * inVertex;
	gl_Position = projection * view * transform * inVertex;
	normal = inNormal;
}
