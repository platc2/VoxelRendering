#version 450

// inputs
layout(location = 0) in vec2 in_vertex;
layout(location = 1) in vec2 in_uv;
// outputs
layout(location = 0) out vec2 out_uv;


void main()
{
	gl_Position = vec4(in_vertex, 0, 1);
	out_uv = in_uv;
}
