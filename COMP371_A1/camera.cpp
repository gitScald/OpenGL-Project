#include "camera.h"

Camera& Camera::get(const glm::vec3& pos, const glm::vec3& up_cam, const glm::vec3& frnt, const glm::vec3& up_wrld) {
    static Camera instance(pos, up_cam, frnt, up_wrld);

    return instance;
}

void Camera::clamp_pitch() {
    // avoid the camera from looking upside down
    if (mouse_pitch < -89.0f)
        mouse_pitch = -89.0f;
    if (mouse_pitch > 89.0f)
        mouse_pitch = 89.0f;
}

void Camera::clamp_fov() {
    // avoid the camera's field of view from exceeding a certain range
    if (field_of_view < MIN_FOV)
        field_of_view = MIN_FOV;
    if (field_of_view > MAX_FOV)
        field_of_view = MAX_FOV;
}

void Camera::move(CAMERA_MOVEMENT direction) {
    // move camera
    switch (direction) {
    case FORWARD:
        position += front * move_speed_curr;
        break;
    case BACKWARD:
        position -= front * move_speed_curr;
        break;
    case LEFTWARD:
        position -= right * move_speed_curr;
        break;
    case RIGHTWARD:
        position += right * move_speed_curr;
        break;
    case UPWARD:
        position += up_world * move_speed_curr;
        break;
    case DOWNWARD:
        position -= up_world * move_speed_curr;
        break;
    }
}

void Camera::update_vectors() {
    // calculate front vector
    front.x = cos(glm::radians(mouse_yaw)) * cos(glm::radians(mouse_pitch));
    front.y = sin(glm::radians(mouse_pitch));
    front.z = sin(glm::radians(mouse_yaw)) * cos(glm::radians(mouse_pitch));
    front = glm::normalize(front);

    // calculate right and up vectors
    right = glm::normalize(glm::cross(front, up_world));
    up_camera = glm::normalize(glm::cross(right, front));
}

const glm::mat4& Camera::projection_matrix() {
    // calculate and return projection matrix
    projection = glm::perspective(glm::radians(field_of_view), ASPECT_RATIO, Z_NEAR, Z_FAR);

    return projection;
}

const glm::mat4& Camera::view_matrix() {
    // calculate and return view matrix
    view = glm::lookAt(position, position + front, up_camera);

    return view;
}

GLfloat Camera::x() const {
    // get current camera x
    return mouse_x;
}

GLfloat Camera::y() const {
    // get current camera y
    return mouse_y;
}

GLfloat Camera::speed_base() const {
    // get current camera base speed
    return move_speed_base;
}

GLfloat Camera::speed_curr() const {
    // get current camera speed
    return move_speed_curr;
}

GLfloat Camera::sensitivity() const {
    // get current camera sensitivity
    return mouse_sensitivity;
}

GLfloat Camera::fov() const {
    // get current camera field of view
    return field_of_view;
}

GLfloat Camera::yaw() const {
    // get current camera yaw angle
    return mouse_yaw;
}

GLfloat Camera::pitch() const {
    // get current camera pitch angle
    return mouse_pitch;
}

bool Camera::synced() const {
    // get whether camera is synchronized with cursor
    return is_synced;
}

void Camera::x(GLfloat value) {
    // set camera x
    mouse_x = value;
}

void Camera::y(GLfloat value) {
    // set camera y
    mouse_y = value;
}

void Camera::speed_base(GLfloat value) {
    // set camera base speed
    move_speed_base = value;
}

void Camera::speed_curr(GLfloat value) {
    // set camera speed
    move_speed_curr = value;
}

void Camera::sensitivity(GLfloat value) {
    // set camera sensitivity
    mouse_sensitivity = value;
}

void Camera::fov(GLfloat value) {
    // set camera field of view
    field_of_view = value;
}

void Camera::yaw(GLfloat value) {
    // set camera yaw angle
    mouse_yaw = value;
}

void Camera::pitch(GLfloat value) {
    // set camera pitch angle
    mouse_pitch = value;
}

void Camera::synced(bool value) {
    // set whether camera is synchronized with cursor
    is_synced = value;
}