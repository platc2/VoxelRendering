#ifndef OPENGLRENDERER_VOXEL_BUFFER_H
#define OPENGLRENDERER_VOXEL_BUFFER_H

#include <cstdint>


namespace voxel
{

/**
 * Class representing a buffer on the physical device.
 */
class Buffer final
{
	// TODO: remove friend declaration
	friend class CMesh;

public:
	/** Type used to represent the size [bytes] of the buffer */
	using size_type = std::uint32_t;

	/**
	 * Usage flag describing what this buffer is used for.
	 */
	enum class Usage
	{
		/** Vertex buffer */
		eVertex,
		/** Index buffer */
		eIndex,
		/** Uniform buffer */
		eUniform,
	};

public:
	/** Size of the buffer [bytes] */
	const Buffer::size_type k_xSize;

	/** Usage of the buffer, i.e. what it is used for */
	const Buffer::Usage k_eUsage;

// --------------------------------------------------
// OpenGL specific members
// --------------------------------------------------
public:
	/** Handle to the OpenGL object */
	std::uint32_t m_uiHandle;

	/** Target, e.g. GL_ARRAY_BUFFER, to bind the buffer to */
	std::uint32_t m_uiTarget;

public:
	/**
	 * Creates and allocates a new buffer.
	 *
	 * @param xSize   Size [in bytes] of the buffer
	 * @param eUsage  Usage flag indicating what the buffer will be used for
	 */
	Buffer(const Buffer::size_type& xSize, const Buffer::Usage& eUsage);

	/**
	 * Virtual destructor, frees the allocated memory on the physical device
	 */
	virtual ~Buffer();

	/**
	 * @return size of the buffer [in bytes]
	 */
	const Buffer::size_type& size() const;

	/**
	 * @return usage of the buffer
	 */
	const Buffer::Usage& usage() const;

	/**
	 * Maps the memory so it can be accessed from this device.
	 */
	void* map();

	/**
	 * Unmaps the memory
	 */
	void unmap();
};

}


#endif
