#ifndef OPENGLRENDERER_VOXEL_RENDERPASS_H
#define OPENGLRENDERER_VOXEL_RENDERPASS_H

#include <cstdint>
#include <string>
#include <vector>

#include "Buffer.h"
#include "VertexAttribute.h"
#include "Texture.h"


namespace voxel
{

/**
 * Class representing a render pass.
 * TODO: Elaborate
 */
class RenderPass final
{
private:
    std::vector<Buffer> k_vecUniformBuffers;

	std::vector<Texture> k_vecTextures;

// --------------------------------------------------
// OpenGL specific members
// --------------------------------------------------
	std::uint32_t m_uiVertexArrayObject;

	std::uint32_t m_uiProgram;

	std::uint32_t m_uiFrameBuffer;

	std::uint32_t m_uiRenderBuffer;

public:
	RenderPass(
		const std::string& strVertexShaderSource,
		const std::string& strFragmentShaderSource,
		const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
		const std::vector<Buffer>& vecUniformBuffers,
		const std::vector<Texture>& vecTextures,
		const std::vector<Texture>& vecAttachments);

	RenderPass(
		const std::string& strVertexShaderSource,
		const std::string& strFragmentShaderSource,
		const std::string& strGeometryShaderSource,
		const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
		const std::vector<Buffer>& vecUniformBuffers,
		const std::vector<Texture>& vecTextures,
		const std::vector<Texture>& vecAttachments);

	RenderPass(
		const std::string& strVertexShaderSource,
		const std::string& strFragmentShaderSource,
		const std::string& strGeometryShaderSource,
		const std::string& strTessellationControlShaderSource,
		const std::string& strTessellationEvaluationShaderSource,
		const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
		const std::vector<Buffer>& vecUniformBuffers,
		const std::vector<Texture>& vecTextures,
		const std::vector<Texture>& vecAttachments);

	void display() const;

private:
	static std::uint32_t createShader(const std::int32_t iShaderType, const std::string& strShaderSource);

	static std::pair<std::uint32_t, std::uint32_t> convertFormat(const VertexAttribute::Format& eFormat);
};

}


#endif
