#include "rendered_entity.h"

// model VAO and VBO initialized to NULL
GLuint RenderedEntity::s_modelVAO = NULL;
GLuint RenderedEntity::s_modelVBO = NULL;

// smooth movement disabled by default
bool RenderedEntity::s_smoothMovement = false;

// base movement speed
GLfloat RenderedEntity::s_speedCurrent = PATHING_SPEED;

glm::mat4 RenderedEntity::getModelMatrix(const glm::mat4& globalModelMatrix) {
    // return model matrix
    return glm::scale(glm::mat4(), m_scalingRelative)
        * globalModelMatrix
        * m_scalingMatrix;
}

glm::mat4 RenderedEntity::getModelMatrixExclScaling(const glm::mat4& jointModelMatrix) {
    // return model matrix (excluding scaling)
    return glm::translate(glm::mat4(), m_position)
        * m_rotationMatrix
        * jointModelMatrix;
}

const glm::mat4& RenderedEntity::getScalingMatrix() const {
    // return scaling matrix
    return m_scalingMatrix;
}

const glm::vec3& RenderedEntity::getScalingRelative() const {
    // return relative scaling
    return m_scalingRelative;
}

const glm::vec3& RenderedEntity::getPivot() const {
    // return entity joint pivot point
    return m_pivot;
}

const glm::vec3& RenderedEntity::getPosition() const {
    // return entity position
    return m_position;
}

const glm::vec4& RenderedEntity::getColorOriginal() const {
    return m_colorOriginal;
}

void RenderedEntity::setSpeedCurrent(GLfloat value) {
    // set movement speed
    s_speedCurrent = value;
}

void RenderedEntity::setColor(const glm::vec4& value) {
    // set entity color
    m_color = value;

    if (!m_colorSet) {
        m_colorOriginal = m_color;
        m_colorSet = true;
    }
}

void RenderedEntity::setFrontVector(const glm::vec3& value) {
    // update front vector
    m_front = value;
}

void RenderedEntity::setPivot(const glm::vec3& value) {
    // set entity joint pivot point
    m_pivot = value;
}

void RenderedEntity::setPosition(const glm::vec3& value) {
    // set entity position
    m_position = value;
}

void RenderedEntity::setRotation(GLfloat value) {
    // set entity position
    m_rotationMatrix = glm::rotate(glm::mat4(),
        glm::radians(value),
        AXIS_Y);
}

void RenderedEntity::setRightVector(const glm::vec3& value) {
    // update entity right vector
    m_right = value;
}

void RenderedEntity::lockRotation(bool toggle) {
    // set entity rotation
    if (!m_rotationSet)
        m_rotationSet = true;
}

void RenderedEntity::lockScaling(bool toggle) {
    // set entity scaling
    if (!m_scaleSet)
        m_scaleSet = true;
}

void RenderedEntity::move(Transform::Displacement direction) {
    // move entity around the grid
    switch (direction) {
    case Transform::Displacement::RANDOM:
        m_position.x = static_cast<GLfloat>(rand()
            % TRANSFORMATION_RANDOM_DISPLACEMENT)
            * m_scalingRelative.x;
        m_position.z = static_cast<GLfloat>(rand()
            % TRANSFORMATION_RANDOM_DISPLACEMENT)
            * m_scalingRelative.x;
        break;
    case Transform::Displacement::FRONT:
        m_position.x += m_front.x
            * s_speedCurrent
            * TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        m_position.z += m_front.z
            * s_speedCurrent
            * TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        break;
    case Transform::Displacement::UP:
        m_position.x += TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        break;
    case Transform::Displacement::DOWN:
        m_position.x -= TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        break;
    case Transform::Displacement::LEFT:
        m_position.z -= TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        break;
    case Transform::Displacement::RIGHT:
        m_position.z += TRANSFORMATION_INCREMENT_TRANSLATION
            * m_scalingRelative.x;
        break;
    }

    clampPosition();
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

void RenderedEntity::toggleSmoothMovement() {
    // set whether smooth movement should be enabled or not
    s_smoothMovement = !s_smoothMovement;
}

void RenderedEntity::render(Rendering::Primitive primitive) const {
    // bind vertex array object
    Shader::bindVAO(m_VAO);

    // render
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

void RenderedEntity::clampPosition() {
    // clamp entity position
    if (m_position.x > POSITION_MAX)
        m_position.x = POSITION_MAX;
    else if (m_position.z > POSITION_MAX)
        m_position.z = POSITION_MAX;

    if (m_position.x < POSITION_MIN)
        m_position.x = POSITION_MIN;
    else if (m_position.z < POSITION_MIN)
        m_position.z = POSITION_MIN;
}

void RenderedEntity::initializeModelEntity(GLfloat vertices[],
    GLuint verticesSize) {
    // generate and bind vertex array object
    if (!s_modelVAO) {
        glGenVertexArrays(1, &s_modelVAO);
        Shader::bindVAO(s_modelVAO);
    }
    m_VAO = s_modelVAO;

    // generate and bind vertex buffect object, buffer data
    if (!s_modelVBO) {
        glGenBuffers(1, &s_modelVBO);
        Shader::bindVBO(s_modelVBO);
        glBufferData(GL_ARRAY_BUFFER,
            verticesSize,
            vertices,
            GL_STATIC_DRAW);
    }
    m_VBO = s_modelVBO;

    // set vertex count
   m_vertexCount = verticesSize / (6 * sizeof(GLfloat));
}

void RenderedEntity::initializeRegularEntity(GLfloat vertices[],
    GLuint verticesSize,
    GLuint indices[],
    GLuint indicesSize) {
    // generate and bind vertex array object
    glGenVertexArrays(1, &m_VAO);
    Shader::bindVAO(m_VAO);

    // generate and bind vertex buffect object, buffer data
    glGenBuffers(1, &m_VBO);
    Shader::bindVBO(m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        vertices,
        GL_STATIC_DRAW);

    // generate and bind element array buffer, buffer data
    if (indices) {
        glGenBuffers(1, &m_EBO);
        Shader::bindEBO(m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indicesSize,
            indices,
            GL_STATIC_DRAW);
    }

    // set vertex count
    if (m_EBO != NULL) {
        m_vertexCount = indicesSize / sizeof(GLuint);
    }
    else
        m_vertexCount = verticesSize / (6 * sizeof(GLfloat));
}