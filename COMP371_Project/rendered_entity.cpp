#include "rendered_entity.h"

glm::mat4 RenderedEntity::getModelMatrix(const glm::mat4& globalModelMatrix) {
    // return model matrix
    return glm::scale(glm::mat4(), m_scalingRelative)
        * globalModelMatrix
        * m_scalingMatrix;
}

glm::mat4 RenderedEntity::getModelMatrixExclScaling(const glm::mat4& jointModelMatrix) {
    // return model matrix (excluding scaling)
    return glm::translate(glm::mat4(), m_position)
        * jointModelMatrix
        * m_rotationMatrix;
}

const glm::mat4& RenderedEntity::getScalingMatrix() const {
    // return scaling matrix
    return m_scalingMatrix;
}

const glm::vec3& RenderedEntity::getScalingRelative() const {
    // return relative scaling
    return m_scalingRelative;
}

void RenderedEntity::setColor(const glm::vec4& value) {
    // set entity color
    m_color = value;
}

void RenderedEntity::setPosition(const glm::vec3& value) {
    // set entity position
    m_position = value;
}

void RenderedEntity::move(Transform::Displacement direction) {
    // move entity around the grid
    switch (direction) {
    case Transform::Displacement::RANDOM:
        m_position.x = static_cast<GLfloat>(rand()
            % TRANSFORMATION_RANDOM_DISPLACEMENT);
        m_position.z = static_cast<GLfloat>(rand()
            % TRANSFORMATION_RANDOM_DISPLACEMENT);
        break;
    case Transform::Displacement::UP:
        m_position.x += TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::DOWN:
        m_position.x -= TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::LEFT:
        m_position.z -= TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::RIGHT:
        m_position.z += TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    }
}

void RenderedEntity::reset() {
    // reset entity position and rotation
    m_position = m_positionOriginal;
    m_rotationMatrix = m_rotationMatrixOriginal;
    m_scalingMatrix = glm::scale(glm::mat4(), m_scalingOriginal);
    m_scalingRelative = glm::vec3(1.0f, 1.0f, 1.0f);
}

void RenderedEntity::rotate(GLfloat angle,
    const glm::vec3& axis) {
    // rotate entity about axis
    if (!m_rotationSet) {
        m_rotationMatrixOriginal *= glm::rotate(glm::mat4(),
            glm::radians(angle),
            axis);
        m_rotationSet = true;
    }

    m_rotationMatrix *= glm::rotate(glm::mat4(),
        glm::radians(angle),
        axis);
}

void RenderedEntity::scale(const glm::vec3& value) {
    // scale entity
    if (!m_scaleSet) {
        m_scalingMatrix *= glm::scale(glm::mat4(), value);
        m_scalingOriginal = value;
        m_scaleSet = true;
    }
    else
        m_scalingRelative = value;
}

void RenderedEntity::translate(const glm::vec3& value) {
    // translate entity
    if (!m_translationSet) {
        m_positionOriginal += value;
        m_translationSet = true;
    }
    m_position += value;
}

void RenderedEntity::render(Rendering::Primitive primitive) const {
    // bind vertex array object and render
    glBindVertexArray(m_VAO);
    switch (primitive) {
    case Rendering::POINTS:
        glPointSize(RENDERING_POINT_SIZE);
        if (m_EBO != NULL)
            glDrawElements(GL_POINTS,
                m_vertexCount,
                GL_UNSIGNED_INT,
                0);
        else
            glDrawArrays(GL_POINTS,
                0,
                m_vertexCount);
        glPointSize(1.0f);
        break;
    case Rendering::LINES:
        if (m_EBO != NULL)
            glDrawElements(GL_TRIANGLES,
                m_vertexCount,
                GL_UNSIGNED_INT,
                0);
        else
            glDrawArrays(GL_TRIANGLES,
                0,
                m_vertexCount);
        break;
    case Rendering::TRIANGLES:
        if (m_EBO != NULL)
            glDrawElements(GL_TRIANGLES,
                m_vertexCount,
                GL_UNSIGNED_INT,
                0);
        else
            glDrawArrays(GL_TRIANGLES,
                0,
                m_vertexCount);
        break;
    }

    // unbind vertex array object
    glBindVertexArray(NULL);
}

void RenderedEntity::setColorShaderAttributes(Shader* shader) const {
    // set shader attributes and color uniforms
    shader->setColorAttributes(m_VAO, m_VBO, m_EBO);
    shader->setUniformVec4(UNIFORM_COLOR,
        m_color);
}

void RenderedEntity::setDepthShaderAttributes(Shader* shader) const {
    // set shader attributes
    shader->setDepthAttributes(m_VAO, m_VBO, m_EBO);
}

void RenderedEntity::initialize(GLfloat vertices[],
    GLuint verticesSize,
    GLuint indices[],
    GLuint indicesSize) {
    // generate and bind vertex array object
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // generate and bind vertex buffect object, buffer data
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        vertices,
        GL_STATIC_DRAW);

    // generate and bind element array buffer, buffer data
    if (indices) {
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indicesSize,
            indices,
            GL_STATIC_DRAW);
    }

    // unbind vertex array object
    glBindVertexArray(NULL);

    // set vertex count
    if (m_EBO != NULL) {
        m_vertexCount = indicesSize / sizeof(GLuint);
    }
    else
        m_vertexCount = verticesSize / (6 * sizeof(GLfloat));
}