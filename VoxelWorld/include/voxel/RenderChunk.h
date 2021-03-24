#ifndef RENDER_CHUNK_H
#define RENDER_CHUNK_H

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

#include "Vertex.h"


class RenderChunk final
{
private:
	/**
	 * Handle to the vertex array object
	 */
	std::uint32_t m_uiVertexArrayObject;

	/**
	 * Handle to the buffer object
	 */
	std::uint32_t m_uiVertexBufferObject;

	/**
	 * Amount of vertices this render object consists of
	 */
	const std::uint32_t k_uiNumVertices;

public:
	/**
	 * Default constructor, required by STL containers
	 */
	RenderChunk() = default;

	/**
	 * Destructor
	 * Frees up allocated GPU memory
	 */
	virtual ~RenderChunk();

	/**
	 * Move constructor
	 * Invalidates GPU object handles to ensure memory remains valid for new object
	 */
	RenderChunk(RenderChunk&& xOther);

	/**
	* Constructs a new render chunk given the vertices
	*
	* @param vecVertices  Vector of vertices which build up this render chunk
	*/
	RenderChunk(const std::vector<Vertex>& vecVertices);

	/**
	 * Renders this chunk
	 *
	 * @param bWireframe  Whether or not the chunk should be rendered in wireframe mode
	 */
    void render(const bool bWireframe) const;
};


#endif // !RENDER_CHUNK_H
