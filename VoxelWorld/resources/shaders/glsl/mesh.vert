#version 410

in vec3 vertex_position;
in vec2 texture_coordinate;

out vec2 frag_texture_coordinate;

uniform mat4 mvp;

void main() {
  frag_texture_coordinate = texture_coordinate;
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
