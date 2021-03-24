#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include "RenderChunkGenerator.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "WorldGenerator.h"

#include <memory>

class WorldRenderer {
public:
    void init();
    void render(glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe);

private:
    std::shared_ptr<Texture> texture;
    ShaderProgram shaderProgram;
    WorldGenerator worldGenerator;
    std::shared_ptr<RenderChunkGenerator> renderChunkGenerator;
};

#endif // !WORLD_RENDERER_H
