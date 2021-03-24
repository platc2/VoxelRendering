#ifndef OPENGLRENDERER_MESH_H
#define OPENGLRENDERER_MESH_H

#include <vector>
#include <cstdint>

#include "Buffer.h"


namespace voxel
{

class CMesh final
{
private:
	std::uint32_t k_uiPrimitiveCount;

	std::uint32_t m_uiVertexArrayObjectHandle;

	Buffer m_xVertexBuffer;
	
	Buffer m_xIndexBuffer;

public:
	static CMesh fromData(const float* const rgVertices, const std::uint32_t uiNumVertices, const std::uint32_t* const rgIndices, const std::uint32_t uiNumIndices);

	virtual ~CMesh();

	CMesh(CMesh&& xOther);

	void render() const;

private:
	CMesh(const float* const rgVertices, const std::uint32_t uiNumVertices, const std::uint32_t* const rgIndices, const std::uint32_t uiNumIndices);
};

}


#endif
