#ifndef CAMERA_H
#define CAMERA_H

// project headers
#include "constants.h"
#include "enums.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// singleton
class Camera {
public:
    Camera() = delete;
    Camera(Camera& camera) = delete;
    Camera(Camera&& camera) = delete;
    Camera& operator=(Camera const& camera) = delete;
    static Camera& get(const glm::vec3& position = CAMERA_POSITION,
        const glm::vec3& front = CAMERA_FRONT,
        const glm::vec3& upCamera = CAMERA_UP_CAMERA,
        const glm::vec3& upWorld = CAMERA_UP_WORLD);

    // getters
    const glm::mat4& getWorldOrientation() const;
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    const glm::vec3& getPosition() const;
    GLuint getViewportWidth() const;
    GLuint getViewportHeight() const;
    GLfloat getMouseX() const;
    GLfloat getMouseY() const;
    GLfloat getYaw() const;
    GLfloat getPitch() const;
    GLfloat getSpeed() const;
    GLfloat getSpeedCurrent() const;
    GLfloat getSensitivity() const;
    GLfloat getFOV() const;
    bool isReady() const;

    // setters
    void setViewportDimensions(GLuint viewportWidth,
        GLuint viewportHeight);
    void setMouseX(GLfloat value);
    void setMouseY(GLfloat value);
    void setYaw(GLfloat value);
    void setPitch(GLfloat value);
    void setSpeedCurrent(GLfloat value);
    void setFOV(GLfloat value);
    void setReady(bool value);

    // movement
    void move(Eye::Displacement direction,
        GLfloat amount);
    void rotate(Eye::Displacement direction,
        GLfloat amount);
    void reset();

    // utilities
    void clampPitch();
    void clampFOV();
    void updateVectors();

private:
    Camera(const glm::vec3& position,
        const glm::vec3& front,
        const glm::vec3& upCamera,
        const glm::vec3& upWorld)
        : m_position{ position },
        m_front{ front },
        m_upCamera{ upCamera },
        m_upWorld{ upWorld } {
        initialize(position,
            front,
            upCamera);
        updateVectors();
    }

    void initialize(const glm::vec3& position,
        const glm::vec3& front,
        const glm::vec3& upCamera);

    static Camera& s_instance;
    glm::mat4 m_worldOrientation;
    glm::vec3 m_position;
    glm::vec3 m_positionOriginal;
    glm::vec3 m_front;
    glm::vec3 m_frontOriginal;
    glm::vec3 m_right;
    glm::vec3 m_upCamera;
    glm::vec3 m_upCameraOriginal;
    glm::vec3 m_upWorld;
    GLuint m_viewportWidth{ SCREEN_WIDTH };
    GLuint m_viewportHeight{ SCREEN_HEIGHT };
    GLfloat m_viewportAspectRatio{ SCREEN_ASPECT_RATIO };
    GLfloat m_mouseX{ SCREEN_WIDTH / 2.0f };
    GLfloat m_mouseY{ SCREEN_HEIGHT / 2.0f };
    GLfloat m_yaw{ CAMERA_YAW };
    GLfloat m_pitch{ CAMERA_PITCH };
    GLfloat m_speed{ CAMERA_SPEED };
    GLfloat m_speedCurrent{ CAMERA_SPEED };
    GLfloat m_sensitivity{ CAMERA_SENSITIVITY };
    GLfloat m_FOV{ CAMERA_FOV_MAX };
    bool m_isReady{ false };
};

#endif // !CAMERA_H