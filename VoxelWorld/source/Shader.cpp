#include "Shader.h"

#include <GL/glew.h>

#include <fmt/format.h>
using namespace fmt;

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

Shader Shader::loadFromFile(const std::string& path, Type shaderType) {
    Shader shader;
    if (shaderType == Type::Fragment) {
        shader.handle = glCreateShader(GL_FRAGMENT_SHADER);
    } else if (shaderType == Type::Vertex) {
        shader.handle = glCreateShader(GL_VERTEX_SHADER);
    } else {
        throw std::runtime_error("Unknown shader type");
    }

    // search paths
    std::ifstream in(format("{}/{}", GLSL_SHADER_PATH, path), std::ios::in);
    if (!in) {
        throw std::runtime_error(format("Failed to read shader file {}", path));
    }

    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const char* csource = source.c_str();

    glShaderSource(shader.handle, 1, &csource, nullptr);
    glCompileShader(shader.handle);
    int params = -1;
    glGetShaderiv(shader.handle, GL_COMPILE_STATUS, &params);
    if (params != GL_TRUE) {
        GLint logLength;
        glGetShaderiv(shader.handle, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> error(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader.handle, logLength, nullptr, &error[0]);
        throw std::runtime_error(std::string(error.begin(), error.end()));
    }
    return shader;
}
