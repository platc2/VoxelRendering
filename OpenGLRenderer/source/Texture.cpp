#include "Texture.h"

#include <GL/glew.h>

using namespace voxel;


Texture::Texture(const std::uint8_t* const pImageData, const std::uint32_t uiWidth, const std::uint32_t uiHeight)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_uiHandle);
	glBindTexture(GL_TEXTURE_2D, m_uiHandle);

	// Optional
//	glTextureParameteri(m_uiHandle, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTextureParameteri(m_uiHandle, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_uiHandle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_uiHandle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateTextureMipmap(m_uiHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, uiWidth, uiHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
}
