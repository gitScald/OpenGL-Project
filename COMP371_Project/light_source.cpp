#include "light_source.h"

const glm::vec3& LightSource::getPosition() const {
    // return light position
    return m_position;
}

const glm::vec3& LightSource::getAmbient() const {
    // return light ambient component
    return m_ambient;
}

const glm::vec3& LightSource::getDiffuse() const {
    // return light diffuse component
    return m_diffuse;
}

const glm::vec3& LightSource::getSpecular() const {
    // return light specular component
    return m_specular;
}

glm::vec3 LightSource::getWorldPosition(
    const glm::mat4& globalModelMatrix) const {
    // return light position in world space
    return glm::vec3(globalModelMatrix
        * glm::vec4(m_position, 1.0f));
}

GLfloat LightSource::getKC() const {
    // return light constant attenuation factor
    return m_KC;
}

GLfloat LightSource::getKL() const {
    // return light linear attenuation factor
    return m_KL;
}

GLfloat LightSource::getKQ() const {
    // return light quadratic attenuation factor
    return m_KQ;
}

GLfloat LightSource::getPlaneNear() const {
    // return light near plane depth
    return m_planeNear;
}

GLfloat LightSource::getPlaneFar() const {
    // return light far plane depth
    return m_planeFar;
}

void LightSource::setPosition(const glm::vec3& value) {
    // set position
    m_position = value;
}

void LightSource::move(Transform::Displacement direction) {
    // move light around the grid
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

void LightSource::reset() {
    // reset light position
    m_position = m_positionOriginal;
}

void LightSource::translate(const glm::vec3& value) {
    // set position value
    m_position += value;
}