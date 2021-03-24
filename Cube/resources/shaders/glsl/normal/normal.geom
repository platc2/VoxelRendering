#version 450

// inputs
layout(triangles) in;
// outputs
layout(line_strip, max_vertices = 6) out;
// uniforms
layout(std140, binding = 0) uniform Matrices
{
	mat4 u_modelViewProjection;
};
layout(std140, binding = 2) uniform GeometryShaderData
{
	float u_normalLength;
};


void main()
{
	// Corners of triangle
	vec3 a = gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[1].gl_Position.xyz;
	vec3 c = gl_in[2].gl_Position.xyz;
	// Calculate center
	vec3 center = (a + b + c) / 3;
	// Calculate normal
	vec3 normal = normalize(cross(a - b, c - b));
	// Calculate line
	gl_Position = u_modelViewProjection * vec4(center, 1);
	EmitVertex();
	gl_Position = u_modelViewProjection * vec4(center + normal * u_normalLength, 1);
	EmitVertex();
	EndPrimitive();
}
