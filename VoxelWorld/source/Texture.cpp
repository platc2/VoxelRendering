#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fmt/format.h>
using namespace fmt;

#include <stdexcept>

Texture Texture::loadFromFile(const std::string& path) {
    Texture texture;

    int width, height, nrChannels;

    std::string absPath = format("{}/{}", TEXTURE_PATH, path);

    unsigned char* data = stbi_load(absPath.c_str(), &width, &height, &nrChannels, STBI_rgb);
    if (!data) {
        throw std::runtime_error(format("Failed to load texture {}", absPath));
    }

    glGenTextures(1, &texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, this->handle);
}
