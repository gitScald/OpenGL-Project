#include "camera.h"

Camera& Camera::get(const glm::vec3& position,
    const glm::vec3& front,
    const glm::vec3& upCamera,
    const glm::vec3& upWorld) {
    // create or return singleton instance
    static Camera s_instance(position, front, upCamera, upWorld);
    

    return s_instance;
}

void Camera::clampPitch() {
    // avoid the camera from looking upside down
    if (m_pitch < -CAMERA_PITCH_MAX)
        m_pitch = -CAMERA_PITCH_MAX;
    else if (m_pitch > CAMERA_PITCH_MAX)
        m_pitch = CAMERA_PITCH_MAX;
}

void Camera::clampFOV() {
    // avoid the camera from zooming excessively
    if (m_FOV < CAMERA_FOV_MIN)
        m_FOV = CAMERA_FOV_MIN;
    else if (m_FOV > CAMERA_FOV_MAX)
        m_FOV = CAMERA_FOV_MAX;
}

const glm::mat4& Camera::getWorldOrientation() const {
    // return world orientation
    return m_worldOrientation;
}

glm::mat4 Camera::getProjectionMatrix() {
    // calculate and return projection matrix
    return glm::perspective(glm::radians(m_FOV),
        m_viewportAspectRatio,
        CAMERA_PLANE_NEAR,
        CAMERA_PLANE_FAR);;
}

glm::mat4 Camera::getViewMatrix() {
    // calculate and return view matrix
    return glm::lookAt(m_position, m_position + m_front, m_upCamera);
}

const glm::vec3& Camera::getPosition() const {
    // returns camera position
    return m_position;
}

GLuint Camera::getViewportWidth() const {
    // return current viewport width
    return m_viewportWidth;
}

GLuint Camera::getViewportHeight() const {
    // return current viewport height
    return m_viewportHeight;
}

GLfloat Camera::getMouseX() const {
    // get current camera x
    return m_mouseX;
}

GLfloat Camera::getMouseY() const {
    // get current camera y
    return m_mouseY;
}

GLfloat Camera::getSpeed() const {
    // get current camera base speed
    return m_speed;
}

GLfloat Camera::getSpeedCurrent() const {
    // get current camera speed
    return m_speedCurrent;
}

GLfloat Camera::getSensitivity() const {
    // get current camera sensitivity
    return m_sensitivity;
}

GLfloat Camera::getFOV() const {
    // get current camera field of view
    return m_FOV;
}

GLfloat Camera::getYaw() const {
    // get current camera yaw angle
    return m_yaw;
}

GLfloat Camera::getPitch() const {
    // get current camera pitch angle
    return m_pitch;
}

bool Camera::isReady() const {
    // get whether camera is synchronized with cursor
    return m_isReady;
}

void Camera::setViewportDimensions(GLuint viewportWidth,
    GLuint viewportHeight) {
    // resize viewport and adjust aspect ratio
    m_viewportWidth = viewportWidth;
    m_viewportHeight = viewportHeight;
    m_viewportAspectRatio = static_cast<GLfloat>(viewportWidth) / viewportHeight;
}

void Camera::setMouseX(GLfloat value) {
    // set camera x
    m_mouseX = value;
}

void Camera::setMouseY(GLfloat value) {
    // set camera y
    m_mouseY = value;
}

void Camera::setYaw(GLfloat value) {
    // set camera yaw angle
    m_yaw = value;
}

void Camera::setPitch(GLfloat value) {
    // set camera pitch angle
    m_pitch = value;
}

void Camera::setSpeedCurrent(GLfloat value) {
    // set camera base speed
    m_speedCurrent = value;
}

void Camera::setFOV(GLfloat value) {
    // set camera field of view
    m_FOV = value;
}

void Camera::setReady(bool value) {
    // set whether camera is synchronized with cursor
    m_isReady = value;
}

void Camera::move(Eye::Displacement direction,
    GLfloat amount) {
    // move camera
    switch (direction) {
    case Eye::Displacement::UP:
        m_position += m_front * m_speedCurrent * amount;
        break;
    case Eye::Displacement::DOWN:
        m_position -= m_front * m_speedCurrent * amount;
        break;
    case Eye::Displacement::LEFT:
        m_position -= m_right * m_speedCurrent * amount;
        break;
    case Eye::Displacement::RIGHT:
        m_position += m_right * m_speedCurrent * amount;
        break;
    }
}

void Camera::rotate(Eye::Displacement direction,
    GLfloat amount) {
    // rotates world view around origin
    switch (direction) {
    case Eye::Displacement::UP:
        m_worldOrientation *= glm::rotate(glm::mat4(),
            glm::radians(amount),
            m_right);
        break;
    case Eye::Displacement::DOWN:
        m_worldOrientation *= glm::rotate(glm::mat4(),
            glm::radians(-amount),
            m_right);
        break;
    case Eye::Displacement::LEFT:
        m_worldOrientation *= glm::rotate(glm::mat4(),
            glm::radians(amount),
            AXIS_Y);
        break;
    case Eye::Displacement::RIGHT:
        m_worldOrientation *= glm::rotate(glm::mat4(),
            glm::radians(-amount),
            AXIS_Y);
        break;
    }

    updateVectors();
}

void Camera::reset() {
    // reset camera to initial parameters
    m_worldOrientation = glm::mat4();
    m_position = m_positionOriginal;
    m_front = m_frontOriginal;
    m_upCamera = m_upCameraOriginal;
    m_yaw = CAMERA_YAW;
    m_pitch = CAMERA_PITCH;
    m_mouseX = SCREEN_WIDTH / 2.0f;
    m_mouseY = SCREEN_HEIGHT / 2.0f;
    m_FOV = CAMERA_FOV_MAX;
    m_isReady = false;

    updateVectors();
}

void Camera::updateVectors() {
    // calculate front vector using yaw and pitch angles
    m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front.y = sin(glm::radians(m_pitch));
    m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    // normalize front vector
    m_front = glm::normalize(m_front);

    // calculate right and up vectors
    m_right = glm::normalize(glm::cross(m_front, m_upWorld));
    m_upCamera = glm::normalize(glm::cross(m_right, m_front));
}

void Camera::initialize(const glm::vec3& position,
    const glm::vec3& front,
    const glm::vec3& upCamera) {
    // set initial parameters
    m_positionOriginal = position;
    m_frontOriginal = front;
    m_upCameraOriginal = upCamera;
}