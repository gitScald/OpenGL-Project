#ifndef MATERIAL_H
#define MATERIAL_H

// project headers
#include "constants.h"
#include "shader.h"

// GLEW
#include <gl/glew.h>

class Material {
public:
    Material()
        : m_diffuse{ NULL },
        m_diffuseUnit{ TEXTURE_UNIT_DIFFUSE },
        m_diffuseIndex{ TEXTURE_INDEX_DIFFUSE },
        m_specular{ NULL },
        m_specularUnit{ TEXTURE_UNIT_SPECULAR },
        m_specularIndex{ TEXTURE_INDEX_SPECULAR },
        m_shininess{ 0.0f } {}
    Material(GLuint textureID, GLfloat shininess)
        : m_diffuse{ textureID },
        m_diffuseUnit{ TEXTURE_UNIT_DIFFUSE },
        m_diffuseIndex{ TEXTURE_INDEX_DIFFUSE },
        m_specular{ textureID },
        m_specularUnit{ TEXTURE_UNIT_SPECULAR },
        m_specularIndex{ TEXTURE_INDEX_SPECULAR },
        m_shininess{ shininess } {}
    Material(GLuint diffuseID, GLuint specularID, GLfloat shininess)
        : m_diffuse{ diffuseID },
        m_diffuseUnit{ TEXTURE_UNIT_DIFFUSE },
        m_diffuseIndex{ TEXTURE_INDEX_DIFFUSE },
        m_specular{ specularID },
        m_specularUnit{ TEXTURE_UNIT_SPECULAR },
        m_specularIndex{ TEXTURE_INDEX_SPECULAR },
        m_shininess{ shininess } {}
    Material(const Material& material)
        : m_diffuse{ material.m_diffuse },
        m_diffuseUnit{ material.m_diffuseUnit },
        m_diffuseIndex{ material.m_diffuseIndex },
        m_specular{ material.m_specular },
        m_specularUnit{ material.m_specularUnit },
        m_specularIndex{ material.m_specularIndex },
        m_shininess{ material.m_shininess } {}
    Material(Material&& material)
        : m_diffuse{ std::move(material.m_diffuse) },
        m_diffuseUnit{ std::move(material.m_diffuseUnit) },
        m_diffuseIndex{ std::move(material.m_diffuseIndex) },
        m_specular{ std::move(material.m_specular) },
        m_specularUnit{ std::move(material.m_specularUnit) },
        m_specularIndex{ std::move(material.m_specularIndex) },
        m_shininess{ std::move(material.m_shininess) } {}
    Material& operator=(Material& material) = delete;

    // utilities
    static void toggleTextures();
    void free() const;
    void use(Shader* shader) const;

private:
    static bool s_texturesEnabled;
    GLuint m_diffuse;
    GLenum m_diffuseUnit;
    GLuint m_diffuseIndex;
    GLuint m_specular;
    GLenum m_specularUnit;
    GLuint m_specularIndex;
    GLfloat m_shininess;
};

#endif // !MATERIAL_H