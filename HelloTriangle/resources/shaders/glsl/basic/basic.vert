#version 440

// inputs
layout(location = 0) in vec4 in_vertex;
layout(location = 1) in vec3 in_color;
// outputs
out ColorData
{
	vec3 color;
};


void main()
{
	gl_Position = in_vertex;
	color = in_color;
}
