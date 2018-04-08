#include "shadow_map.h"

GLuint ShadowMap::getDepthTextureID() const {
    // return depth texture id
    return m_depthTextureID;
}

GLuint ShadowMap::getFBOID() const {
    // return FBO id
    return m_FBO;
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
    // set shader uniforms
    Shader::useProgram(m_shaderDebug.getProgramID());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthTextureID);
    m_shaderDebug.setUniformUInt(UNIFORM_SHADOW_DEPTH_TEXTURE,
        0);

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthTextureID);
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