#include "voxel/WorldRenderer.h"
#include "TextureAtlas.h"
#include "voxel/RenderChunkGenerator.h"

#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

const int CAMERA_CHUNK_DISTANCE = 15;

void WorldRenderer::init() {
    texture = std::make_shared<Texture>(Texture::loadFromFile("texture_atlas.gif"));
    renderChunkGenerator = std::make_shared<RenderChunkGenerator>(1000);

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.link();
}

void WorldRenderer::render(glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);

    this->shaderProgram.use();
    this->texture->bind();

    for (int x = currentX - CAMERA_CHUNK_DISTANCE; x < currentX + CAMERA_CHUNK_DISTANCE; x++) {
        for (int z = currentZ - CAMERA_CHUNK_DISTANCE; z < currentZ + CAMERA_CHUNK_DISTANCE; z++) {
            const auto position = glm::ivec3(x, 1, z);
            const auto floatPosition = glm::vec3(position);

            // check if current chunk is behind the camera, if yes, skip rendering it
            // vector from camera position to current block position
            const glm::vec3 cameraRelativeVector = floatPosition - cameraPos;
            // if dot product is smaller than 0.0 (angle between the two vectors is greater than perpendicular),
            // block is behind the camera
            if (glm::dot(cameraRelativeVector, cameraFront) < 0.0f) {
                continue;
            }

            auto chunk = worldGenerator.getChunk(position);
            const auto renderChunk = renderChunkGenerator->fromChunk(position, (*chunk), worldGenerator);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(floatPosition));
            auto mvp = vp * modelMatrix;
            this->shaderProgram.setUniform("mvp", mvp);
            renderChunk->render(wireframe);
        }
    }
}
