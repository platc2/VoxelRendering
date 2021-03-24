#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_color;

layout(location = 0) out vec3 color;

uniform mat4 mvp;

void main() {
  color = vertex_color;
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
