#version 410

in vec2 frag_texture_coordinate;

out vec4 fragmentColor;

uniform sampler2D model_texture;

void main() {
  fragmentColor = texture(model_texture, frag_texture_coordinate);
}
