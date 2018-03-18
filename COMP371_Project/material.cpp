#include "material.h"

// textures enabled by default
bool Material::s_texturesEnabled = true;

void Material::toggleTextures() {
    // set whether textures should be enabled or not
    s_texturesEnabled = !s_texturesEnabled;
}

void Material::free() const {
    // free textures
    if (m_diffuse)
        glDeleteTextures(1, &m_diffuse);
    if (m_specular)
        glDeleteTextures(1, &m_specular);
}

void Material::use(Shader* shader) const {
    // binds textures and sets shader uniforms
    shader->use();
    shader->setUniformFloat(UNIFORM_MATERIAL_SHININESS, m_shininess);

    glActiveTexture(m_diffuseUnit);
    if (s_texturesEnabled)
        glBindTexture(GL_TEXTURE_2D, m_diffuse);
    else
        glBindTexture(GL_TEXTURE_2D, NULL);
    shader->setUniformUInt(UNIFORM_MATERIAL_DIFFUSE, m_diffuseIndex);

    glActiveTexture(m_specularUnit);
    if (s_texturesEnabled)
        glBindTexture(GL_TEXTURE_2D, m_specular);
    else
        glBindTexture(GL_TEXTURE_2D, NULL);
    shader->setUniformUInt(UNIFORM_MATERIAL_SPECULAR, m_specularIndex);
}