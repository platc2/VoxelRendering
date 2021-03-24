#version 440

// inputs
in ColorData
{
	vec3 color;
};
// outputs
layout(location = 0) out vec4 out_color;
// uniforms
layout(std140, binding = 0) uniform Data
{
	float gamma;
};


void main()
{
	out_color = vec4(color, 1);
	// sRGB color correction
	out_color.rgb = pow(out_color.rgb, vec3(1.0 / gamma));
}
