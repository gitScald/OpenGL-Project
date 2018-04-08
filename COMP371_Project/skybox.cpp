#include "skybox.h"

void Skybox::free() const {
    // free resources
    m_shader.free();
    glDeleteTextures(1, &m_texture);
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}

void Skybox::render(const glm::mat4& globalModelMatrix,
    const glm::vec3& cameraPosition) const {
    // disable depth test
    glDisable(GL_DEPTH_TEST);

    // compute model matrix
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(modelMatrix, cameraPosition)
        * globalModelMatrix;

    // set shader uniforms
    Shader::useProgram(m_shader.getProgramID());
    m_shader.setUniformMat4(UNIFORM_MATRIX_MODEL,
        modelMatrix);

    // activate and bind cubemap texture
    glActiveTexture(m_textureIndex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    m_shader.setUniformUInt(UNIFORM_SKYBOX_TEXTURE,
        m_textureUnit);

    // bind vertex array object and render
    Shader::bindVAO(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // reenable depth test
    glEnable(GL_DEPTH_TEST);
}

void Skybox::updateViewMatrix(const glm::mat4& viewMatrix) const {
    // update view matrix
    Shader::useProgram(m_shader.getProgramID());
    m_shader.setUniformMat4(UNIFORM_MATRIX_VIEW,
        viewMatrix);
}

void Skybox::updateProjectionMatrix(const glm::mat4& projectionMatrix) const {
    // update projection matrix
    Shader::useProgram(m_shader.getProgramID());
    m_shader.setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        projectionMatrix);
}

void Skybox::initialize(const std::string (&path_textures)[6]) {
    // vertex data
    GLfloat vertices[] = {
        // back face
        // position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // front face
        // position
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // left face
        // position
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // right face
        // position
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        // bottom face
        // position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        // top face
        // position
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
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

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shader.getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);

    // generate and bind texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    // set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    // load images and generate mipmaps
    GLuint successCount{ 0 };
    int width, height, channels;
    stbi_uc* data;
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
    for (GLuint i = 0; i != 6; ++i) {
        std::cout << "Loading texture from file: \"" << path_textures[i]
            << "\"..." << std::endl;
        data = stbi_load(path_textures[i].c_str(), &width, &height, &channels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            ++successCount;
        }
        else
            std::cerr << ">>> Failed to load texture from file \"" 
                << path_textures[i] << "\"" << std::endl;

        // free image data
        stbi_image_free(data);
    }

    if (successCount == 6)
        std::cout << "Skybox texture loading successful." << std::endl;
    std::cout << std::endl;
}