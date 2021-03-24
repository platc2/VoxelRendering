#ifndef RENDER_CHUNK_GENERATOR_H
#define RENDER_CHUNK_GENERATOR_H

#include <GL/glew.h>

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>
#include <list>

#include "Mesh.h"
#include "Vertex.h"
#include "LimitedUnorderedMap.h"
#include "voxel/RenderChunk.h"
#include "voxel/WorldGenerator.h"


/*
 * Generate mesh and texture coordinate from a Chunk
 */
class RenderChunkGenerator final
{
private:

	/**
	 * Vertices for a standard cube mesh in NDC.
	 */
	static const std::vector<Vertex> k_vecCubeMesh;

	/**
	 * Cache for the rendered chunks.
	 */
	LimitedUnorderedMap<glm::ivec3, RenderChunk> chunkCache;

public:
	/**
	 * Constructor
	 */
    RenderChunkGenerator(std::size_t cacheSize);

    const std::shared_ptr<RenderChunk> fromChunk(const glm::ivec3 position, const Chunk& chunk, WorldGenerator& worldGenerator);
};


#endif // !RENDER_CHUNK_GENERATOR_H
