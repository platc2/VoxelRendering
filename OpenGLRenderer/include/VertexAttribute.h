#ifndef OPENGLRENDERER_VOXEL_VERTEXATTRIBUTE_H
#define OPENGLRENDERER_VOXEL_VERTEXATTRIBUTE_H

#include <cstdint>


namespace voxel
{

/**
 * Describes a vertex attribute
 */
class VertexAttribute final
{
public:
	enum Format
	{
		/** 1 channel, 32bit each */
		eR32F,
		/** 2 channels, 32bit each */
		eRG32F,
		/** 3 channels, 32bit each */
		eRGB32F,
		/** 4 channels, 32bit each */
		eRGBA32F,
		/** 1 channel, 8bit each */
		eR8,
		/** 2 channel, 8bit each */
		eRG8,
		/** 3 channel, 8bit each */
		eRGB8,
		/** 4 channel, 8bit each */
		eRGBA8,
	};

private:
	Format m_eFormat;

	std::uint32_t m_uiOffset;

public:
	VertexAttribute() = default;

	VertexAttribute(const Format& eFormat, const std::uint32_t uiOffset);

	Format format() const;

	std::uint32_t offset() const;
};

}


#endif
