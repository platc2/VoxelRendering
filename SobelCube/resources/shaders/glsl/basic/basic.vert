#version 450

// inputs
layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_uv;
// outputs
layout(location = 0) out vec2 out_uv;
// uniforms
layout(std140, binding = 0) uniform Matrices
{
	mat4 u_modelViewProjection;
};


void main()
{
	gl_Position = u_modelViewProjection * vec4(in_vertex, 1);
	out_uv = in_uv;
}
