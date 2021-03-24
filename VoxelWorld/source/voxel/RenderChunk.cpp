#include "voxel/RenderChunk.h"


/**
 * Destructor
 * Frees up allocated GPU memory.
 */
RenderChunk::~RenderChunk()
{
	glDeleteBuffers(1, &m_uiVertexBufferObject);
	glDeleteVertexArrays(1, &m_uiVertexArrayObject);
}


/**
 * Move constructor
 * Invalidates GPU object handles to ensure memory remains valid for new object.
 */
RenderChunk::RenderChunk(RenderChunk&& xOther)
	: m_uiVertexArrayObject(xOther.m_uiVertexArrayObject), m_uiVertexBufferObject(xOther.m_uiVertexBufferObject), k_uiNumVertices(xOther.k_uiNumVertices)
{
	xOther.m_uiVertexArrayObject = 0;
	xOther.m_uiVertexBufferObject = 0;
}


/**
 * Constructs a new render chunk given the vertices.
 *
 * @param vecVertices  Vector of vertices which build up this render chunk
 */
RenderChunk::RenderChunk(const std::vector<Vertex>& vecVertices)
	: m_uiVertexArrayObject(0), m_uiVertexBufferObject(0), k_uiNumVertices(static_cast<std::uint32_t>(vecVertices.size()))
{
	glGenVertexArrays(1, &m_uiVertexArrayObject);
	glBindVertexArray(m_uiVertexArrayObject);

	glGenBuffers(1, &m_uiVertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_uiVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * k_uiNumVertices, vecVertices.data(), GL_STATIC_DRAW);
	// Vertex coordinates
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texturePosition)));
}


/**
* Renders this chunk
*
* @param bWireframe  Whether or not the chunk should be rendered in wireframe mode
*/
void RenderChunk::render(const bool bWireframe) const
{
	glBindVertexArray(static_cast<GLuint>(m_uiVertexArrayObject));
	glPolygonMode(GL_FRONT_AND_BACK, bWireframe ? GL_LINE : GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(k_uiNumVertices));
}
