#version 330

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
	// fragColor = max(0, dot(normalize(vec3(-57, -250, -20)), normalize(normal))) * vec3(0.8, 0.6, 0.2);
	fragColor = color;
	gl_Position = projection * view * vec4(vertex, 1.0f);
}