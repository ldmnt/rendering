#version 330

layout (location = 0) in vec4 inVertex;
layout (location = 1) in vec4 inNormal;

out vec4 normal;
out vec4 fragPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main() {
	fragPos = transform * inVertex;
	gl_Position = projection * view * transform * inVertex;
	normal = inNormal;
}
