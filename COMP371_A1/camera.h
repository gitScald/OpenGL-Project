#ifndef CAMERA_H
#define CAMERA_H

// project headers
#include "globals.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// camera movement values
const enum CAMERA_MOVEMENT {
    FORWARD,
    BACKWARD,
    LEFTWARD,
    RIGHTWARD,
    UPWARD,
    DOWNWARD
};

class Camera {  // singleton
public:
    Camera() = delete;
    Camera(Camera& cam) = delete;
    Camera& operator=(Camera const& cam) = delete;
    static Camera& get(const glm::vec3& pos = glm::vec3(0.0f, 50.0f, 0.0f), const glm::vec3& up_cam = glm::vec3(0.0f, 0.0f, -1.0f), const glm::vec3& frnt = glm::vec3(0.0f, 0.0f, -1.0f), const glm::vec3& up_wrld = glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::mat4& projection_matrix();
    const glm::mat4& view_matrix();
    GLfloat x() const;
    GLfloat y() const;
    GLfloat speed_base() const;
    GLfloat speed_curr() const;
    GLfloat sensitivity() const;
    GLfloat fov() const;
    GLfloat yaw() const;
    GLfloat pitch() const;
    bool synced() const;
    void x(GLfloat value);
    void y(GLfloat value);
    void speed_base(GLfloat value);
    void speed_curr(GLfloat value);
    void sensitivity(GLfloat value);
    void fov(GLfloat value);
    void yaw(GLfloat value);
    void pitch(GLfloat value);
    void synced(bool value);

    void clamp_pitch();
    void clamp_fov();
    void move(CAMERA_MOVEMENT direction);
    void update_vectors();

private:
    Camera(const glm::vec3& pos, const glm::vec3& up_cam, const glm::vec3& frnt = glm::vec3(0.0f, 0.0f, -1.0f), const glm::vec3& up_wrld = glm::vec3(0.0f, 1.0f, 0.0f))
        : position{ pos }, up_camera{ up_cam }, front{ frnt }, up_world{ up_wrld } {
        update_vectors();
    }
    static Camera& instance;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up_camera;
    glm::vec3 up_world;
    glm::mat4 projection;
    glm::mat4 view;
    GLfloat mouse_x{ SCREEN_WIDTH / 2.0f };
    GLfloat mouse_y{ SCREEN_HEIGHT / 2.0f };
    GLfloat mouse_yaw{ INIT_YAW };
    GLfloat mouse_pitch{ INIT_PITCH };
    GLfloat move_speed_base{ INIT_SPEED };
    GLfloat move_speed_curr{ INIT_SPEED };
    GLfloat mouse_sensitivity{ INIT_SENSITIVITY };
    GLfloat field_of_view{ MAX_FOV };
    bool is_synced{ false };
};

#endif // !CAMERA_H