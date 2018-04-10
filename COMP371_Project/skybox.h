#ifndef SKYBOX_H
#define SKYBOX_H

// project headers
#include "shader.h"
#include "vertex_loader.h"

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// STB image loading library
#include <stb_image/stb_image.h>

class Skybox {
public:
    Skybox() = delete;
    Skybox(const std::string& path_vertex,
        const std::string& path_fragment,
        const std::string (&path_textures)[6])
        : m_shader{ path_vertex, path_fragment } {
        initialize(path_textures);
    }
    Skybox(const Skybox& skybox)
        : m_shader{ skybox.m_shader },
        m_texture{ skybox.m_texture },
        m_textureUnit{ skybox.m_textureUnit },
        m_textureIndex{ skybox.m_textureIndex },
        m_VAO{ skybox.m_VAO },
        m_VBO{ skybox.m_VBO } {}
    Skybox(Skybox&& skybox)
        : m_shader{ std::move(skybox.m_shader) },
        m_texture{ std::move(skybox.m_texture) },
        m_textureUnit{ std::move(skybox.m_textureUnit) },
        m_textureIndex{ std::move(skybox.m_textureIndex) },
        m_VAO{ std::move(skybox.m_VAO) },
        m_VBO{ std::move(skybox.m_VBO) } {}
    Skybox& operator=(Skybox& skybox) = delete;

    // utilities
    void free() const;
    void render(const glm::mat4& globalModelMatrix,
        const glm::vec3& cameraPosition) const;
    void updateFogProperties(bool fogEnabled) const;
    void updateLightColor(const glm::vec4& value) const;
    void updateViewMatrix(const glm::mat4& viewMatrix) const;
    void updateProjectionMatrix(const glm::mat4& projectionMatrix) const;

private:
    void initialize(const std::string (&path_textures)[6]);

    Shader m_shader;
    GLuint m_texture;
    GLenum m_textureUnit{ TEXTURE_UNIT_SKYBOX };
    GLuint m_textureIndex{ TEXTURE_INDEX_SKYBOX };
    GLuint m_VAO;
    GLuint m_VBO;
};

#endif // !SKYBOX_H
