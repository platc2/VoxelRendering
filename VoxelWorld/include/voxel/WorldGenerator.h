#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <unordered_map>
#include <memory>

#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>

#include "PerlinNoise.h"
#include "Tensor3.h"

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = 64;

const int WATER_HEIGHT = CHUNK_HEIGHT / 5;

const int BLOCK_AIR = 0;

using Chunk = Tensor3<char, CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE>;

class WorldGenerator {
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunkCache;
    siv::PerlinNoise noise;

public:
    WorldGenerator();
    std::shared_ptr<Chunk> getChunk(const glm::ivec3& position);
};

#endif // !WORLD_GENERATOR_H
