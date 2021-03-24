#include "Buffer.h"

#include <GL/glew.h>

using namespace voxel;


/**
 * Creates and allocates a new buffer.
 *
 * @param xSize   Size [in bytes] of the buffer
 * @param eUsage  Usage flag indicating what the buffer will be used for
 */
Buffer::Buffer(const Buffer::size_type& xSize, const Buffer::Usage& eUsage)
	: k_xSize(xSize), k_eUsage(eUsage)
{
	switch(k_eUsage)
	{
	case Usage::eVertex:
		m_uiTarget = GL_ARRAY_BUFFER;
		break;
	case Usage::eIndex:
		m_uiTarget = GL_ELEMENT_ARRAY_BUFFER;
		break;
	case Usage::eUniform:
		m_uiTarget = GL_UNIFORM_BUFFER;
		break;
	}

	glCreateBuffers(1, &m_uiHandle);
	glBindBuffer(m_uiTarget, m_uiHandle);
	// These flags make changes to the buffer instant, e.g. they will be applied on the next draw call
	const GLbitfield xUniformBufferFlags = GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(m_uiTarget, xSize, nullptr, GL_MAP_WRITE_BIT | (k_eUsage == Usage::eUniform ? xUniformBufferFlags : 0));
}


/**
 * Virtual destructor, frees the allocated memory on the physical device
 */
Buffer::~Buffer()
{
}


/**
 * @return size of the buffer [in bytes]
 */
const Buffer::size_type& Buffer::size() const
{
	return k_xSize;
}


/**
 * @return usage of the buffer
 */
const Buffer::Usage& Buffer::usage() const
{
	return k_eUsage;
}


/**
 * Maps the memory so it can be accessed from this device.
 */
void* Buffer::map()
{
	// We must ensure the buffer was bound before we map it
	glBindBuffer(m_uiTarget, m_uiHandle);
	return glMapBuffer(m_uiTarget, GL_WRITE_ONLY);
}


/**
 * Unmaps the memory
 */
void Buffer::unmap()
{
	glUnmapBuffer(m_uiTarget);
}
