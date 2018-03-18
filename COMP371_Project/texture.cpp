#include "texture.h"

GLuint Texture::getID() const {
    // return texture id
    return m_ID;
}

void Texture::free() const {
    // free texture
    glDeleteTextures(1, &m_ID);
}

void Texture::initialize(const std::string& path,
    GLenum format,
    GLenum wrapping,
    GLenum filtering) {    
    // generate and bind texture
    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    // set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

    // load image and generate mipmaps
    std::cout << "Loading texture from file: \"" << path << "\"..." << std::endl;
    int width, height, channels;
    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGB,
            width,
            height,
            0,
            format,
            GL_UNSIGNED_BYTE,
            data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture loading successful.";
    }
    else
        std::cerr << ">>> Failed to load texture from file \"" << path << "\"";
    std::cout << std::endl << std::endl;

    // free texture data
    stbi_image_free(data);
}