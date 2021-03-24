#ifndef MESH_H
#define MESH_H

#include "Vertex.h"

#include <string>
#include <vector>

#include <GL/glew.h>

class Mesh {
public:
    static Mesh loadFromFile(const std::string& path);
    void draw(bool wireframe);
    const std::vector<Vertex>& getVertices() const;
    const std::vector<unsigned int>& getIndices() const;

private:
    Mesh() = default;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

#endif // !MESH_H
