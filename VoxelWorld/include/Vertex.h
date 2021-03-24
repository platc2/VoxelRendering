#ifndef VERTEX_H
#define VERTEX_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Vertex {
    Vertex(glm::vec3 p, glm::vec2 t) : position(p), texturePosition(t){};
    glm::vec3 position = glm::vec3();
    glm::vec2 texturePosition = glm::vec2();
};

#endif // !VERTEX_H
