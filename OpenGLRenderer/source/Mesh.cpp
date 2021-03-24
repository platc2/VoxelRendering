#include "Mesh.h"

#include <cstring>

#include <GL/glew.h>


using namespace voxel;


CMesh CMesh::fromData(const float* const rgVertices, const std::uint32_t uiNumVertices, const std::uint32_t* const rgIndices, const std::uint32_t uiNumIndices)
{
	return CMesh(rgVertices, uiNumVertices, rgIndices, uiNumIndices);
}


CMesh::~CMesh()
{
	glDeleteVertexArrays(1, &m_uiVertexArrayObjectHandle);
}


CMesh::CMesh(CMesh&& xOther)
	: k_uiPrimitiveCount(xOther.k_uiPrimitiveCount), m_uiVertexArrayObjectHandle(xOther.m_uiVertexArrayObjectHandle),
	m_xVertexBuffer(xOther.m_xVertexBuffer), m_xIndexBuffer(xOther.m_xIndexBuffer)
{
	xOther.m_uiVertexArrayObjectHandle = 0;
}


void CMesh::render() const
{
	glBindVertexArray(m_uiVertexArrayObjectHandle);
	glBindVertexBuffer(0, m_xVertexBuffer.m_uiHandle, 0, sizeof(float) * 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_xIndexBuffer.m_uiHandle);
	glDrawElements(GL_TRIANGLES, k_uiPrimitiveCount, GL_UNSIGNED_INT, nullptr);
}


CMesh::CMesh(const float* const rgVertices, const std::uint32_t uiNumVertices, const std::uint32_t* const rgIndices, const std::uint32_t uiNumIndices)
	: k_uiPrimitiveCount(uiNumIndices), m_uiVertexArrayObjectHandle(0),
	m_xVertexBuffer(sizeof(float) * uiNumVertices, Buffer::Usage::eVertex),
	m_xIndexBuffer(sizeof(std::uint32_t) * uiNumIndices, Buffer::Usage::eIndex)
{
	glGenVertexArrays(1, &m_uiVertexArrayObjectHandle);
	glBindVertexArray(m_uiVertexArrayObjectHandle);
	// Vertex format
	glEnableVertexAttribArray(0);
	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	// Vertex buffer
	void* const pVertexBufferMemory = m_xVertexBuffer.map();
	memcpy(pVertexBufferMemory, rgVertices, m_xVertexBuffer.size());
	m_xVertexBuffer.unmap();
	// Index buffer
	void* const pIndexBufferMemory = m_xIndexBuffer.map();
	memcpy(pIndexBufferMemory, rgIndices, m_xIndexBuffer.size());
	m_xIndexBuffer.unmap();
}
