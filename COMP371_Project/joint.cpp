#include "joint.h"

glm::mat4 Joint::getModelMatrix() const {
    // return current joint transformation matrix
    return glm::translate(glm::mat4(), -m_position)
        * m_rotationMatrix
        * glm::translate(glm::mat4(), m_position);
}

GLfloat Joint::getRotation() const {
    // return current joint rotation
    return m_rotationAngle;
}

void Joint::reset() {
    // reset rotation
    m_rotationMatrix = glm::mat4();
    m_rotationAngle = 0.0f;
}

void Joint::rotate(GLfloat angle, const glm::vec3& axis) {
    // rotate child entity about axis
    m_rotationAngle += angle;
    clampRotation();

    m_rotationMatrix = glm::rotate(glm::mat4(),
        glm::radians(m_rotationAngle),
        AXIS_Z);
}

bool Joint::contains(RenderedEntity* entity) const {
    // return whether joint uses entity
    return m_child == entity;
}

void Joint::clampRotation() {
    // clamp joint rotation
    if (m_rotationAngle > m_rotationMax)
        m_rotationAngle = m_rotationMax;
    else if (m_rotationAngle < m_rotationMin)
        m_rotationAngle = m_rotationMin;
}