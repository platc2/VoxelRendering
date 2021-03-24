#ifndef OPENGLRENDERER_VOXEL_TEXTURE_H
#define OPENGLRENDERER_VOXEL_TEXTURE_H

#include <cstdint>


namespace voxel
{

/**
 * Class representing a Texture
 */
class Texture final
{
// --------------------------------------------------
// OpenGL specific members
// --------------------------------------------------
public:
	std::uint32_t m_uiHandle;

public:
	Texture(const std::uint8_t* const pImageData, const std::uint32_t uiWidth, const std::uint32_t uiHeight);
};

}


#endif
