#version 450

layout(location = 0) in vec2 in_uv;
// outputs
layout(location = 0) out vec4 color;
// uniforms
layout(std140, binding = 0) uniform Kernel
{
	mat3 filterKernel;
};
layout(std140, binding = 1) uniform ScreenSize
{
	vec2 screenSize;
};
layout(binding = 0) uniform sampler2D sceneTexture;


void main()
{
	vec2 offset = 1.0 / screenSize;
	vec2 offsets[9] = vec2[](
		vec2(-offset.x,  offset.y), // top-left
		vec2( 0.0f,      offset.y), // top-center
		vec2( offset.x,  offset.y), // top-right
		vec2(-offset.x,  0.0f),     // center-left
		vec2( 0.0f,      0.0f),     // center-center
		vec2( offset.x,  0.0f),     // center-right
		vec2(-offset.x, -offset.y), // bottom-left
		vec2( 0.0f,     -offset.y), // bottom-center
		vec2( offset.x, -offset.y)  // bottom-right    
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; ++i)
	{
		sampleTex[i] = vec3(texture(sceneTexture, in_uv + offsets[i]));
	}
	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; ++i)
	{
		int filterKernelX = i % 3;
		int filterKernelY = i / 3;
		col += sampleTex[i] * filterKernel[filterKernelX][filterKernelY];
	}

	color = vec4(col, 0.0);
}
