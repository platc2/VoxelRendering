#version 450

in VertexInfo
{
	layout(location = 0) in vec3 fragPosition;
	layout(location = 1) in vec2 fragTexture;
	layout(location = 2) in mat3 fragTBN;
};


layout(location = 0) out vec4 color;


layout(binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;
// layout(binding = 2) uniform sampler2D texNormal;


void main()
{
	// The normal part does nothing but add overhead to the calculation
/*
	vec3 normal = texture(texNormal, fragTexture).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	normal = normalize(fragTBN * normal);
*/

	color = texture(texSampler, fragTexture);
}
