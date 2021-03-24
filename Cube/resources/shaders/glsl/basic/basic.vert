#version 450

// inputs
layout(location = 0) in vec3 in_vertex;
// uniforms
layout(std140, binding = 0) uniform Matrices
{
	mat4 u_modelViewProjection;
};


void main()
{
	gl_Position = u_modelViewProjection * vec4(in_vertex, 1);
}
