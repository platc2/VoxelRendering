#include "RenderPass.h"

#ifdef _MSC_VER
	#include <ciso646>
#endif
#include <vector>
#include <stdexcept>
#include <GL/glew.h>

using namespace voxel;


RenderPass::RenderPass(
	const std::string& strVertexShaderSource,
	const std::string& strFragmentShaderSource,
	const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
	const std::vector<Buffer>& vecUniformBuffers,
	const std::vector<Texture>& vecTextures,
	const std::vector<Texture>& vecAttachments)
	: RenderPass(strVertexShaderSource, strFragmentShaderSource, "", vecVertexAttributes, vecUniformBuffers, vecTextures, vecAttachments)
{
	// Delegates to other constructor, with empty geometry shader instead
}


RenderPass::RenderPass(
	const std::string& strVertexShaderSource,
	const std::string& strFragmentShaderSource,
	const std::string& strGeometryShaderSource,
	const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
	const std::vector<Buffer>& vecUniformBuffers,
	const std::vector<Texture>& vecTextures,
	const std::vector<Texture>& vecAttachments)
	: RenderPass(strVertexShaderSource, strFragmentShaderSource, "", "", "", vecVertexAttributes, vecUniformBuffers, vecTextures, vecAttachments)
{
	// Delegates to other constructor, with empty geometry & tessellation shader instead
}


RenderPass::RenderPass(
	const std::string& strVertexShaderSource,
	const std::string& strFragmentShaderSource,
	const std::string& strGeometryShaderSource,
	const std::string& strTessellationControlShaderSource,
	const std::string& strTessellationEvaluationShaderSource,
	const std::vector<std::pair<std::uint8_t, VertexAttribute>>& vecVertexAttributes,
	const std::vector<Buffer>& vecUniformBuffers,
	const std::vector<Texture>& vecTextures,
	const std::vector<Texture>& vecAttachments)
	: k_vecUniformBuffers{vecUniformBuffers}, k_vecTextures{vecTextures}, m_uiFrameBuffer{0}
{
	glCreateVertexArrays(1, &m_uiVertexArrayObject);
	// Set vertex attributes binding
	std::uint8_t ui8AttributeIndex = 0;
	for(const std::pair<std::uint8_t, VertexAttribute>& xEntry : vecVertexAttributes)
	{
		const std::uint8_t& ui8BindingIndex = xEntry.first;
		const VertexAttribute& xAttribute = xEntry.second;
		const std::pair<std::uint32_t, std::uint32_t> xConvertedFormat = convertFormat(xAttribute.format());
		glEnableVertexArrayAttrib(m_uiVertexArrayObject, ui8AttributeIndex);
		glVertexArrayAttribFormat(m_uiVertexArrayObject, ui8AttributeIndex, xConvertedFormat.first, xConvertedFormat.second, GL_FALSE, xAttribute.offset());
		glVertexArrayAttribBinding(m_uiVertexArrayObject, ui8AttributeIndex, ui8BindingIndex);
		++ui8AttributeIndex;
	}

	// Create OpenGL shader program
	const GLuint uiVertexShader = createShader(GL_VERTEX_SHADER, strVertexShaderSource);
	const GLuint uiFragmentShader = createShader(GL_FRAGMENT_SHADER, strFragmentShaderSource);
	const bool bHasGeometryShader = !strGeometryShaderSource.empty();
	const GLuint uiGeometryShader = bHasGeometryShader? createShader(GL_GEOMETRY_SHADER, strGeometryShaderSource) : 0;
	const bool bHasTessellationControlShader = !strTessellationControlShaderSource.empty();
	const GLuint uiTessellationControlShader = bHasTessellationControlShader ? createShader(GL_TESS_CONTROL_SHADER, strTessellationControlShaderSource) : 0;
	const bool bHasTessellationEvaluationShader = !strTessellationEvaluationShaderSource.empty();
	const GLuint uiTessellationEvaluationShader = bHasTessellationEvaluationShader ? createShader(GL_TESS_EVALUATION_SHADER, strTessellationEvaluationShaderSource) : 0;

	const GLuint uiProgramHandle = glCreateProgram();
	glAttachShader(uiProgramHandle, uiVertexShader);
	glAttachShader(uiProgramHandle, uiFragmentShader);
	if(bHasGeometryShader)
	{
		glAttachShader(uiProgramHandle, uiGeometryShader);
	}
	if(bHasTessellationControlShader)
	{
		glAttachShader(uiProgramHandle, uiTessellationControlShader);
	}
	if(bHasTessellationEvaluationShader)
	{
		glAttachShader(uiProgramHandle, uiTessellationEvaluationShader);
	}
	glLinkProgram(uiProgramHandle);
	GLint iStatus;
	glGetProgramiv(uiProgramHandle, GL_LINK_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetProgramiv(uiProgramHandle, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetProgramInfoLog(uiProgramHandle, iInfoLogLength, nullptr, vecInfoLog.data());
		const std::string strInfoLog = std::string(std::begin(vecInfoLog), std::end(vecInfoLog));
		throw std::runtime_error("Failed to link program: " + strInfoLog);
	}

	m_uiProgram = uiProgramHandle;

	if(not vecAttachments.empty())
	{
		glGenFramebuffers(1, &m_uiFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiFrameBuffer);

		std::uint32_t uiAttachmentIndex = 0;
		for(const Texture& xAttachment : vecAttachments)
		{
			glActiveTexture(GL_TEXTURE0 + uiAttachmentIndex);
			glBindTexture(GL_TEXTURE_2D, xAttachment.m_uiHandle);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + uiAttachmentIndex, GL_TEXTURE_2D, xAttachment.m_uiHandle, 0);
		}

		glGenRenderbuffers(1, &m_uiRenderBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, m_uiRenderBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1024, 1024);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_uiRenderBuffer);
	}
}


void RenderPass::display() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiFrameBuffer);

	glBindVertexArray(m_uiVertexArrayObject);
	glUseProgram(m_uiProgram);
	// Bind all uniform buffers for this render pass
    std::uint32_t uiUniformBufferIndex = 0;
    for (const Buffer& xUniformBuffer : k_vecUniformBuffers)
	{
		glBindBufferRange(GL_UNIFORM_BUFFER, uiUniformBufferIndex++, xUniformBuffer.m_uiHandle, 0, xUniformBuffer.size());
	}

	std::uint32_t uiTextureIndex = 0;
	for(const Texture& xTexture : k_vecTextures)
	{
		glActiveTexture(GL_TEXTURE0 + uiTextureIndex++);
		glBindTexture(GL_TEXTURE_2D, xTexture.m_uiHandle);
	}
}


std::uint32_t RenderPass::createShader(const std::int32_t iShaderType, const std::string& strShaderSource)
{
	const GLuint uiShader = glCreateShader(iShaderType);
	const char* const szSource = strShaderSource.c_str();
	glShaderSource(uiShader, 1, &szSource, nullptr);
	glCompileShader(uiShader);
	GLint iStatus;
	glGetShaderiv(uiShader, GL_COMPILE_STATUS, &iStatus);
	if(iStatus == GL_FALSE)
	{
		GLint iInfoLogLength;
		glGetShaderiv(uiShader, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		std::vector<char> vecInfoLog(iInfoLogLength);
		glGetShaderInfoLog(uiShader, iInfoLogLength, nullptr, vecInfoLog.data());
		const std::string strInfoLog = std::string(std::begin(vecInfoLog), std::end(vecInfoLog));
		throw std::runtime_error("Failed to compile shader: " + strInfoLog);
	}

	return uiShader;
}


std::pair<std::uint32_t, std::uint32_t> RenderPass::convertFormat(const VertexAttribute::Format& eFormat)
{
	using Format = VertexAttribute::Format;
	// (size, type)
	switch(eFormat)
	{
	case Format::eR32F:
		return std::make_pair(1, GL_FLOAT);
	case Format::eRG32F:
		return std::make_pair(2, GL_FLOAT);
	case Format::eRGB32F:
		return std::make_pair(3, GL_FLOAT);
	case Format::eRGBA32F:
		return std::make_pair(4, GL_FLOAT);
	case Format::eR8:
		return std::make_pair(1, GL_BYTE);
	case Format::eRG8:
		return std::make_pair(2, GL_BYTE);
	case Format::eRGB8:
		return std::make_pair(3, GL_BYTE);
	case Format::eRGBA8:
		return std::make_pair(4, GL_BYTE);
	default:
		// TODO: Don't return value, either throw exception or use mapping array
		return std::make_pair(0, GL_BYTE);
	}
}
