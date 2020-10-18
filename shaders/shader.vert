#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
	fragColor = (max(0, dot(normalize(-vec3(-4, -4, -4)), normalize(normal))) + 0.2) * color;
	gl_Position = projection * view * vec4(vertex, 1.0f);
}