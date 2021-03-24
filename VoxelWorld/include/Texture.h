#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <string>

class Texture {
public:
    static Texture loadFromFile(const std::string& path);

    void bind();

private:
    Texture() = default;
    GLuint handle = 0;
};

#endif // !TEXTURE_H
