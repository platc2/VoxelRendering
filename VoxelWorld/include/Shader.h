#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <GL/glew.h>

class Shader
{
    friend class ShaderProgram;

private:
    Shader() = default;
    GLuint handle = 0;

public:
    enum class Type
    {
        Vertex,
        Fragment
    };
    static Shader loadFromFile(const std::string &path, Type shaderType);
};

#endif
