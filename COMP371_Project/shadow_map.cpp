#include "shadow_map.h"

// initial shadow parameters
GLfloat ShadowMap::s_biasMax = SHADOW_BIAS_MAX;
GLfloat ShadowMap::s_biasMin = SHADOW_BIAS_MIN;
GLfloat ShadowMap::s_gridFactor = SHADOW_GRID_FACTOR;
GLfloat ShadowMap::s_gridOffset = SHADOW_GRID_OFFSET;
GLuint ShadowMap::s_gridSamples = SHADOW_GRID_SAMPLES;

GLfloat ShadowMap::getBiasMax() {
    // get max shadow map bias
    return s_biasMax;
}

GLfloat ShadowMap::getBiasMin() {
    // get min shadow map bias
    return s_biasMin;
}

GLfloat ShadowMap::getGridFactor() {
    // get grid sampling factor
    return s_gridFactor;
}

GLfloat ShadowMap::getGridOffset() {
    // get grid sampling offset
    return s_gridOffset;
}

GLuint ShadowMap::getGridSamples() {
    // get grid sample count
    return s_gridSamples;
}

GLuint ShadowMap::getDepthTextureID() const {
    // return depth texture id
    return m_depthTextureID;
}

GLuint ShadowMap::getFBOID() const {
    // return FBO id
    return m_FBO;
}

void ShadowMap::adjustBiasMax(Shadows::Tweak mod) {
    // adjust max shadow map bias
    switch (mod) {
    case Shadows::INCREASE:
        s_biasMax += SHADOW_INCREMENT_BIAS;
        break;
    case Shadows::DECREASE:
        s_biasMax -= SHADOW_INCREMENT_BIAS;
        break;
    }
    clampBiasMax();
    std::cout << "Shadow map bias max: " << s_biasMax << std::endl;
}

void ShadowMap::adjustBiasMin(Shadows::Tweak mod) {
    // adjust min shadow map bias
    switch (mod) {
    case Shadows::INCREASE:
        s_biasMin += SHADOW_INCREMENT_BIAS;
        break;
    case Shadows::DECREASE:
        s_biasMin -= SHADOW_INCREMENT_BIAS;
        break;
    }
    clampBiasMin();
    std::cout << "Shadow map bias min: " << s_biasMin << std::endl;
}

void ShadowMap::adjustGridFactor(Shadows::Tweak mod) {
    // adjust shadow map sampling grid factor
    switch (mod) {
    case Shadows::INCREASE:
        s_gridFactor += SHADOW_INCREMENT_GRID_FACTOR;
        break;
    case Shadows::DECREASE:
        s_gridFactor -= SHADOW_INCREMENT_GRID_FACTOR;
        break;
    }
    clampGridFactor();
    std::cout << "Shadow map sampling grid factor: " << s_gridFactor << std::endl;
}

void ShadowMap::adjustGridOffset(Shadows::Tweak mod) {
    // adjust shadow map sampling grid offset
    switch (mod) {
    case Shadows::INCREASE:
        s_gridOffset += SHADOW_INCREMENT_GRID_OFFSET;
        break;
    case Shadows::DECREASE:
        s_gridOffset -= SHADOW_INCREMENT_GRID_OFFSET;
        break;
    }
    clampGridOffset();
    std::cout << "Shadow map sampling grid offset: " << s_gridOffset << std::endl;
}

void ShadowMap::adjustGridSamples(Shadows::Tweak mod) {
    // adjust shadow map sampling grid samples
    switch (mod) {
    case Shadows::INCREASE:
        s_gridSamples += SHADOW_INCREMENT_GRID_SAMPLES;
        break;
    case Shadows::DECREASE:
        s_gridSamples -= SHADOW_INCREMENT_GRID_SAMPLES;
        break;
    }
    clampGridSamples();
    std::cout << "Shadow map grid samples: " << s_gridSamples << std::endl;
}

void ShadowMap::free() const {
    // free resources
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteFramebuffers(1, &m_FBO);
    glDeleteTextures(1, &m_depthTextureID);
}

void ShadowMap::render() const {
    // set texture properties
    Shader::useProgram(m_shaderDebug.getProgramID());
    Shader::activateTextureUnit(GL_TEXTURE0);
    Shader::bindCubemapTexture(m_depthTextureID);

    // render depth texture on debug quad
    Shader::bindVAO(m_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void ShadowMap::initialize() {
    // generate and bind framebuffer
    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    
    // generate cubemap texture
    glGenTextures(1, &m_depthTextureID);
    Shader::bindCubemapTexture(m_depthTextureID);
    for (GLuint face{ 0 }; face != 6; ++face)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
            0,
            GL_DEPTH_COMPONENT,
            SHADOW_DEPTH_TEXTURE_WIDTH,
            SHADOW_DEPTH_TEXTURE_HEIGHT,
            0,
            GL_DEPTH_COMPONENT,
            GL_FLOAT,
            NULL);

    // set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_WRAP_R,
        GL_CLAMP_TO_BORDER);

    // set texture border
    glTexParameterfv(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_BORDER_COLOR,
        SHADOW_BORDER_COLOR);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST);

    // use texture as depth attachment
    glFramebufferTexture(GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        m_depthTextureID,
        0);

    // framebuffer will not use a color buffer, use GL_NONE
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // check the framebuffer for problems
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)
        == GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Shadow map framebuffer complete."
        << std::endl << std::endl;
    else
        std::cout << ">>> Shadow map framebuffer incomplete."
        << std::endl << std::endl;

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);

    // set shader uniform
    Shader::useProgram(m_shaderDebug.getProgramID());
    m_shaderDebug.setUniformUInt(UNIFORM_SHADOW_DEPTH_TEXTURE,
        0);
}

void ShadowMap::initializeDebugQuad() {
    // vertex positions for framebuffer quad
    GLfloat vertices[] = {
        // position   // texture
        0.4f, 1.0f,   0.0f, 1.0f,
        0.4f, 0.4f,   0.0f, 0.0f,
        1.0f, 0.4f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_VAO);
    Shader::bindVAO(m_VAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_VBO);
    Shader::bindVBO(m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    // generate and bind element array buffer object, buffer data
    glGenBuffers(1, &m_EBO);
    Shader::bindEBO(m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW);

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shaderDebug.getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);
    GLuint textureLocation = glGetAttribLocation(m_shaderDebug.getProgramID(),
        ATTRIBUTE_TEXTURE.c_str());
    glVertexAttribPointer(textureLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(GLfloat),
        (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureLocation);
}

void ShadowMap::clampBiasMax() {
    // clamp grid sample count
    if (s_biasMax >= 5.0f)
        s_biasMax = 5.0f;
    if (s_biasMax < 0.0f)
        s_biasMax = 0.0f;
}

void ShadowMap::clampBiasMin() {
    // clamp grid sample count
    if (s_biasMin >= s_biasMax)
        s_biasMin = s_biasMax;
    if (s_biasMin < 0.0f)
        s_biasMin = 0.0f;
}

void ShadowMap::clampGridFactor() {
    // clamp grid sample count
    if (s_gridFactor >= 200.0f)
        s_gridFactor = 200.0f;
    if (s_gridFactor < 1.0f)
        s_gridFactor = 1.0f;
}

void ShadowMap::clampGridOffset() {
    // clamp grid sample count
    if (s_gridOffset >= 5.0f)
        s_gridOffset = 5.0f;
    if (s_gridOffset < 0.0f)
        s_gridOffset = 0.0f;
}

void ShadowMap::clampGridSamples() {
    // clamp grid sample count
    if (s_gridSamples >= 256)
        s_gridSamples = 256;
    if (s_gridSamples < 0)
        s_gridSamples = 0;
}