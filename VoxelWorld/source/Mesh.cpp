#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stdexcept>

#include <fmt/format.h>
using namespace fmt;

Mesh Mesh::loadFromFile(const std::string& path) {
    Mesh model;
    Assimp::Importer importer;

    std::string absPath = format("{}/{}", MESH_PATH, path);

    const aiScene* scene = importer.ReadFile(absPath.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw std::runtime_error(format("Failed to load model from {}: {}", absPath, importer.GetErrorString()));
    }

    // load vertex positions
    auto mesh = scene->mMeshes[0];
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        model.vertices.push_back(Vertex(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                                        glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)));
    }

    // load vertex incides
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            model.indices.push_back(face.mIndices[j]);
        }
    }

    glGenVertexArrays(1, &model.vao); // one attribute
    glBindVertexArray(model.vao);

    // load vertex positions
    glGenBuffers(1, &model.vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model.vertices.size(), &model.vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    // load vertex incides
    glGenBuffers(1, &model.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * model.indices.size(), &model.indices[0],
                 GL_STATIC_DRAW);

    // load texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texturePosition));

    return model;
}

void Mesh::draw(bool wireframe) {
    glBindVertexArray(this->vao);
    glDrawElements(wireframe ? GL_LINES : GL_TRIANGLES, static_cast<GLsizei>(this->indices.size()), GL_UNSIGNED_INT,
                   nullptr);
}

const std::vector<Vertex>& Mesh::getVertices() const {
    return vertices;
}

const std::vector<unsigned int>& Mesh::getIndices() const {
    return indices;
}
