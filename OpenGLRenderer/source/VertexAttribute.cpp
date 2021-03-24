#include "VertexAttribute.h"

using namespace voxel;


VertexAttribute::VertexAttribute(const Format& eFormat, const std::uint32_t uiOffset)
	: m_eFormat(eFormat),
	m_uiOffset(uiOffset)
{
}


VertexAttribute::Format VertexAttribute::format() const
{
	return m_eFormat;
}


std::uint32_t VertexAttribute::offset() const
{
	return m_uiOffset;
}
