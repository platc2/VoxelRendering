#include "ShaderProgram.h"

#include <stdexcept>
#include <string>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram() {
    this->handle = glCreateProgram();
}

void ShaderProgram::attachShader(Shader shader) {
    glAttachShader(this->handle, shader.handle);
}

void ShaderProgram::link() {
    glLinkProgram(this->handle);
    int params = -1;
    glGetProgramiv(this->handle, GL_LINK_STATUS, &params);
    if (params != GL_TRUE) {
        GLint logLength;
        glGetProgramiv(this->handle, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> error(static_cast<size_t>(logLength));
        glGetProgramInfoLog(this->handle, logLength, nullptr, &error[0]);
        throw std::runtime_error(std::string(error.begin(), error.end()));
    }
}

void ShaderProgram::use() {
    glUseProgram(this->handle);
}

void ShaderProgram::setUniform(const std::string& uniform, glm::mat4 data) {
    int location = glGetUniformLocation(this->handle, uniform.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(data));
}

void ShaderProgram::setUniform(const std::string& uniform, int data) {
    int location = glGetUniformLocation(this->handle, uniform.c_str());
    glUniform1i(location, data);
}
