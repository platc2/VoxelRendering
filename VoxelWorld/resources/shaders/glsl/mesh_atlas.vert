#version 410

in vec3 vertex_position;
in vec2 texture_coordinate;

out vec2 frag_texture_coordinate;

uniform mat4 mvp;
uniform int texture_offset;

void main() {
  float x = (texture_coordinate.x + float(texture_offset)) / 24.0;
  frag_texture_coordinate = vec2(x, texture_coordinate.y);
  gl_Position = mvp * vec4(vertex_position, 1.0);
}
