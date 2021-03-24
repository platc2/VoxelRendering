#include "voxel/RenderChunkGenerator.h"
#include "TextureAtlas.h"
#include "voxel/WorldGenerator.h"

/**
 * Vertices for a standard cube mesh in NDC.
 */
const std::vector<Vertex> RenderChunkGenerator::k_vecCubeMesh = {
    // top
    Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313589f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600387f)),
    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313589f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312806f)),
    Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601170f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312806f)),

    Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313589f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600387f)),
    Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601170f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312806f)),
    Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.601170f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600387f)),

    // bottom
    Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.927016f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.650563f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.927016f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.938145f)),
    Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.639435f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.938145f)),

    Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.927016f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.650563f)),
    Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.639435f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.938145f)),
    Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.639435f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.650563f)),

    // right
    Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.602356f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600009f)),
    Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.602502f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312428f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.890083f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312574f)),

    Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.602356f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600009f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.890083f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.312574f)),
    Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.889937f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.600155f)),

    // left
    Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.025300f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.601718f)),
    Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.025154f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.314136f)),
    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.312735f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.313990f)),

    Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.025300f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.601718f)),
    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.312735f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.313990f)),
    Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.312881f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.601572f)),

    // front
    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313669f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.311237f)),
    Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.314044f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.023656f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.601625f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.024030f)),

    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313669f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.311237f)),
    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.601625f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.024030f)),
    Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601250f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.311611f)),

    // back
    Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.313199f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.889921f)),
    Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313199f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.602340f)),
    Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.600780f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.602340f)),

    Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.313199f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.889921f)),
    Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.600780f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.602340f)),
    Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.600780f / static_cast<float>(TEXTURE_ATLAS_SIZE), 0.889921f))};

static bool needsRender(const Chunk& chunk, const int x, const int y, const int z, const glm::ivec3& position,
                        WorldGenerator& worldGenerator) {

    if (x < 0) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(-1, 0, 0));
        return (*adjacentChunk)(CHUNK_SIZE - 1, y, z) == BLOCK_AIR;
    }
    if (y < 0) {
        // never render a block below lowest point
        return false;
    }
    if (z < 0) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(0, 0, -1));
        return (*adjacentChunk)(x, y, CHUNK_SIZE - 1) == BLOCK_AIR;
    }

    if (x == CHUNK_SIZE) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(1, 0, 0));
        return (*adjacentChunk)(0, y, z) == BLOCK_AIR;
    }
    if (y == CHUNK_HEIGHT) {
        // there will never be a block heigher than this, always render
        return true;
    }
    if (z == CHUNK_SIZE) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(0, 0, 1));
        return (*adjacentChunk)(x, y, 0) == BLOCK_AIR;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}

RenderChunkGenerator::RenderChunkGenerator(std::size_t cacheSize) : chunkCache(cacheSize) {
}

const std::shared_ptr<RenderChunk> RenderChunkGenerator::fromChunk(const glm::ivec3 position, const Chunk& chunk,
                                                   WorldGenerator& worldGenerator) {
    // Return the chunk from the cache if it exists

    auto cacheEntry = chunkCache.get(position);
    if (cacheEntry) {
        return cacheEntry;
    }

    std::vector<Vertex> vs;

    const float textureAtlasSize = static_cast<float>(TEXTURE_ATLAS_SIZE);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    continue;
                }

                // position offset
                const auto po = glm::vec3(x, y, z);

                // texture coordinate offset
                const auto tco = glm::vec2(static_cast<float>(chunk(x, y, z) - 1) / textureAtlasSize, 0.0f);

                const float cs = static_cast<float>(CHUNK_SIZE);

                // top
                if (needsRender(chunk, x, y + 1, z, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[0].position + po) / cs, k_vecCubeMesh[0].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[1].position + po) / cs, k_vecCubeMesh[1].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[2].position + po) / cs, k_vecCubeMesh[2].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[3].position + po) / cs, k_vecCubeMesh[3].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[4].position + po) / cs, k_vecCubeMesh[4].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[5].position + po) / cs, k_vecCubeMesh[5].texturePosition + tco));
                }

                // bottom
                if (needsRender(chunk, x, y - 1, z, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[6].position + po) / cs, k_vecCubeMesh[6].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[7].position + po) / cs, k_vecCubeMesh[7].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[8].position + po) / cs, k_vecCubeMesh[8].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[9].position + po) / cs, k_vecCubeMesh[9].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[10].position + po) / cs, k_vecCubeMesh[10].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[11].position + po) / cs, k_vecCubeMesh[11].texturePosition + tco));
                }

                // right
                if (needsRender(chunk, x + 1, y, z, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[12].position + po) / cs, k_vecCubeMesh[12].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[13].position + po) / cs, k_vecCubeMesh[13].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[14].position + po) / cs, k_vecCubeMesh[14].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[15].position + po) / cs, k_vecCubeMesh[15].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[16].position + po) / cs, k_vecCubeMesh[16].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[17].position + po) / cs, k_vecCubeMesh[17].texturePosition + tco));
                }

                // left
                if (needsRender(chunk, x - 1, y, z, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[18].position + po) / cs, k_vecCubeMesh[18].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[19].position + po) / cs, k_vecCubeMesh[19].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[20].position + po) / cs, k_vecCubeMesh[20].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[21].position + po) / cs, k_vecCubeMesh[21].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[22].position + po) / cs, k_vecCubeMesh[22].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[23].position + po) / cs, k_vecCubeMesh[23].texturePosition + tco));
                }

                // front
                if (needsRender(chunk, x, y, z + 1, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[24].position + po) / cs, k_vecCubeMesh[24].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[25].position + po) / cs, k_vecCubeMesh[25].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[26].position + po) / cs, k_vecCubeMesh[26].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[27].position + po) / cs, k_vecCubeMesh[27].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[28].position + po) / cs, k_vecCubeMesh[28].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[29].position + po) / cs, k_vecCubeMesh[29].texturePosition + tco));
                }

                // back
                if (needsRender(chunk, x, y, z - 1, position, worldGenerator)) {
                    vs.push_back(Vertex((k_vecCubeMesh[30].position + po) / cs, k_vecCubeMesh[30].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[31].position + po) / cs, k_vecCubeMesh[31].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[32].position + po) / cs, k_vecCubeMesh[32].texturePosition + tco));

                    vs.push_back(Vertex((k_vecCubeMesh[33].position + po) / cs, k_vecCubeMesh[33].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[34].position + po) / cs, k_vecCubeMesh[34].texturePosition + tco));
                    vs.push_back(Vertex((k_vecCubeMesh[35].position + po) / cs, k_vecCubeMesh[35].texturePosition + tco));
                }
            }
        }
    }

    // Create the chunk, add it to the cache
    auto renderChunk = std::make_shared<RenderChunk>(vs);
    chunkCache.set(position, renderChunk);

    return renderChunk;
}
