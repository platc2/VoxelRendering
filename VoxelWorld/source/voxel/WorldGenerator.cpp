#include "voxel/WorldGenerator.h"
#include "TextureAtlas.h"

const double NOISE_SCALE = 0.1;

WorldGenerator::WorldGenerator() : noise(1) {
}

std::shared_ptr<Chunk> WorldGenerator::getChunk(const glm::ivec3& position) {
    auto cacheEntry = chunkCache.find(position);
    if (cacheEntry != chunkCache.end()) {
        return cacheEntry->second;
    }

    auto chunk = std::make_shared<Chunk>();

    // basic world generation
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            const double scaledX = (double(x) / double(CHUNK_SIZE) + double(position.x)) * NOISE_SCALE;
            const double scaledY = (double(z) / double(CHUNK_SIZE) + double(position.z)) * NOISE_SCALE;

            const double value = noise.octaveNoise0_1(scaledX, scaledY, 8);

            const int height = static_cast<int>(value * CHUNK_HEIGHT);
            for (int y = 0; y < height; y++) {
                if (y > CHUNK_HEIGHT * 0.7) {
                    (*chunk)(x, y, z) = TextureAtlas::SNOW;
                } else if (y > CHUNK_HEIGHT * 0.5) {
                    (*chunk)(x, y, z) = TextureAtlas::STONE_04;
                } else {
                    (*chunk)(x, y, z) = TextureAtlas::GROUND_EARTH;
                }
            }
            for (int y = height; y < CHUNK_HEIGHT; y++) {
                (*chunk)(x, y, z) = BLOCK_AIR;
            }
        }
    }

    // water
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y <= WATER_HEIGHT; y++) {
                if ((*chunk)(x, y, z) == BLOCK_AIR) {
                    (*chunk)(x, y, z) = TextureAtlas::WATER;
                }
            }
            if ((*chunk)(x, WATER_HEIGHT, z) == TextureAtlas::GROUND_EARTH && (*chunk)(x, WATER_HEIGHT + 1, z) == BLOCK_AIR) {
                (*chunk)(x, WATER_HEIGHT, z) = TextureAtlas::WATER;
            }
        }
    }

    chunkCache[position] = chunk;
    return chunk;
}
