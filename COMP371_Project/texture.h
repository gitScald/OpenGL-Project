#ifndef TEXTURE_H
#define TEXTURE_H

// project headers
#include "enums.h"

// GLEW
#include <gl/glew.h>

// STB image loading library
#include <stb_image/stb_image.h>

// C++ standard library headers
#include <iostream>
#include <string>

class Texture {
public:
    Texture(const std::string& path,
        GLenum format,
        GLenum wrapping,
        GLenum filtering) {
        initialize(path, format, wrapping, filtering);
    }
    Texture(const Texture& texture)
        : m_ID{ texture.m_ID } {}
    Texture(Texture&& texture)
        : m_ID{ std::move(texture.m_ID) } {}
    Texture& operator=(Texture& tex) = delete;

    // getters
    GLuint getID() const;

    // utilities
    void free() const;

private:
    void initialize(const std::string& path,
        GLenum format,
        GLenum wrapping,
        GLenum filtering);

    GLuint m_ID;
};

#endif // !TEXTURE_H