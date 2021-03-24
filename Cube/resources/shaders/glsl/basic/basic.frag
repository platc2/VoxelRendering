#version 450

// outputs
layout(location = 0) out vec4 color;
// uniforms
layout(std140, binding = 1) uniform Colors
{
	vec3 u_color;
};


void main()
{
	color = vec4(u_color, 1);
}
