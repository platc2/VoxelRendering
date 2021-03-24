#version 450

layout(location = 0) in vec2 in_uv;
// outputs
layout(location = 0) out vec4 color;
// uniforms
layout(std140, binding = 1) uniform Colors
{
	vec3 u_color;
};
layout(std140, binding = 2) uniform TextureSwitch
{
	int value;
};
layout(binding = 0) uniform sampler2D floorTexture;
layout(binding = 1) uniform sampler2D cubeTexture;


void main()
{
	switch(value)
	{
	case 0:
		color = texture(floorTexture, in_uv);
		break;
	case 1:
		color = texture(cubeTexture, in_uv);
		break;
	}
}
