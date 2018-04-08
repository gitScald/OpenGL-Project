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
    Shader::useProgram(shader->getProgramID());
    shader->setUniformFloat(UNIFORM_MATERIAL_SHININESS, m_shininess);

    Shader::activateTextureUnit(m_diffuseUnit);
    if (s_texturesEnabled)
        Shader::bind2DTexture(m_diffuse);
    else
        Shader::bind2DTexture(NULL);
    shader->setUniformUInt(UNIFORM_MATERIAL_DIFFUSE, m_diffuseIndex);

    Shader::activateTextureUnit(m_specularUnit);
    if (s_texturesEnabled)
        Shader::bind2DTexture(m_specular);
    else
        Shader::bind2DTexture(NULL);
    shader->setUniformUInt(UNIFORM_MATERIAL_SPECULAR, m_specularIndex);
}