#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inTangent;
layout(location = 3) in vec3 inBitangent;
layout(location = 4) in vec2 inTexture;


out VertexInfo
{
	layout(location = 0) out vec3 fragPosition;
	layout(location = 1) out vec2 fragTexture;
	layout(location = 2) out mat3 fragTBN;
};


out gl_PerVertex
{
	vec4 gl_Position;
};


layout(std140, binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 projection;
} ubo;


void main()
{
	gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
	fragPosition = vec3(ubo.model * vec4(inPosition, 1.0));
	fragTexture = inTexture;

	vec3 T = normalize(vec3(ubo.model * vec4(inTangent, 0.0)));
	vec3 B = normalize(vec3(ubo.model * vec4(inBitangent, 0.0)));
	vec3 N = normalize(vec3(ubo.model * vec4(inNormal, 0.0)));
	fragTBN = transpose(mat3(T, B, N));
}
