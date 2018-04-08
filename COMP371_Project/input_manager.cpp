#include "input_manager.h"

InputManager& InputManager::get() {
    // create or return singleton instance
    static InputManager s_instance;

    return s_instance;
}

void InputManager::processCursor(GLFWwindow* window,
    double posX,
    double posY) const {
    // process cursor input
    GLfloat posX_ = static_cast<GLfloat>(posX);
    GLfloat posY_ = static_cast<GLfloat>(posY);
    GLfloat offX = posX_ - Camera::get().getMouseX();
    GLfloat offY = Camera::get().getMouseY() - posY_;

    // synchronize camera with initial cursor position
    if (!Camera::get().isReady()) {
        Camera::get().setMouseX(posX_);
        Camera::get().setMouseY(posY_);
        Camera::get().setReady(false);
    }

    // adjust camera position
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)
        && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        Camera::get().move(Eye::Displacement::RIGHT, offX);
    }

    // adjust camera zoom
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)
        && !glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        Camera::get().setFOV(Camera::get().getFOV() - offY);
        Camera::get().clampFOV();
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)
        || (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)
            && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))) {
        // update camera x and y
        Camera::get().setMouseX(posX_);
        Camera::get().setMouseY(posY_);

        // update camera angles
        offX *= Camera::get().getSensitivity();
        offY *= Camera::get().getSensitivity();
        Camera::get().setYaw(Camera::get().getYaw() + offX);
        Camera::get().setPitch(Camera::get().getPitch() + offY);
        Camera::get().clampPitch();

        // update camera vectors
        Camera::get().updateVectors();
    }

    // update renderer view and projection matrices
    Renderer::get().updateViewMatrix();
    Renderer::get().updateProjectionMatrix();
}

void InputManager::processKeyboard(GLFWwindow* window,
    int key,
    int scanCode,
    int action,
    int mods) const {
    // close window
    if (key == GLFW_KEY_ESCAPE
        && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        std::cout << std::endl << "Closing window..." << std::endl;
    }

    // change rendering primitive
    if (key == GLFW_KEY_P
        && action == GLFW_PRESS
        && Renderer::get().getPrimitive() != Rendering::POINTS) {
        Renderer::get().setPrimitive(Rendering::POINTS);
        std::cout << "Rendering primitive changed to: POINTS"
            << std::endl;
    }
    if (key == GLFW_KEY_L
        && action == GLFW_PRESS
        && Renderer::get().getPrimitive() != Rendering::LINES) {
        Renderer::get().setPrimitive(Rendering::LINES);
        std::cout << "Rendering primitive changed to: LINES"
            << std::endl;
    }
    if (key == GLFW_KEY_T
        && action == GLFW_PRESS
        && Renderer::get().getPrimitive() != Rendering::TRIANGLES) {
        Renderer::get().setPrimitive(Rendering::TRIANGLES);
        std::cout << "Rendering primitive changed to: TRIANGLES"
            << std::endl;
    }

    // world orientation
    if (key == GLFW_KEY_UP
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Camera::get().rotate(Eye::Displacement::UP,
            TRANSFORMATION_INCREMENT_ROTATION);
    if (key == GLFW_KEY_DOWN
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Camera::get().rotate(Eye::Displacement::DOWN,
            TRANSFORMATION_INCREMENT_ROTATION);
    if (key == GLFW_KEY_LEFT
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Camera::get().rotate(Eye::Displacement::LEFT,
            TRANSFORMATION_INCREMENT_ROTATION);
    if (key == GLFW_KEY_RIGHT
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Camera::get().rotate(Eye::Displacement::RIGHT,
            TRANSFORMATION_INCREMENT_ROTATION);

    // camera reset
    if (key == GLFW_KEY_HOME
        && action == GLFW_PRESS) {
        Camera::get().reset();
        Renderer::get().updateViewMatrix();
        Renderer::get().updateProjectionMatrix();
    }

    // model scaling
    if (key == GLFW_KEY_U
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Renderer::get().scaleModel(0,
            Transform::Scale::INCREASE);
    if (key == GLFW_KEY_J
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Renderer::get().scaleModel(0,
            Transform::Scale::DECREASE);

    // model movement and rotation
    if (key == GLFW_KEY_END
        && action == GLFW_PRESS)
        Renderer::get().resetModel(0);
    if (key == GLFW_KEY_SPACE
        && (action == GLFW_PRESS
            || action == GLFW_REPEAT))
        Renderer::get().moveModel(0,
            Transform::Displacement::RANDOM);
    if (mods == GLFW_MOD_SHIFT) {
        if (key == GLFW_KEY_W
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().moveModel(0,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_S
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().moveModel(0,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_A
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().moveModel(0,
                Transform::Displacement::LEFT);
        if (key == GLFW_KEY_D
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().moveModel(0,
                Transform::Displacement::RIGHT);
    }
    else {
        if (key == GLFW_KEY_W
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModel(0,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_S
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModel(0,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_A
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModel(0,
                Transform::Displacement::LEFT);
        if (key == GLFW_KEY_D
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModel(0,
                Transform::Displacement::RIGHT);
    }

    // joint rotations
    if (mods == GLFW_MOD_SHIFT) {
        if (key == GLFW_KEY_0
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                0,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_1
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                1,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_2
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                2,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_3
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                3,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_4
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                4,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_5
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                5,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_6
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                6,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_7
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                7,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_8
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                8,
                Transform::Displacement::DOWN);
        if (key == GLFW_KEY_9
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                9,
                Transform::Displacement::DOWN);
    }
    else {
        if (key == GLFW_KEY_0
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                0,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_1
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                1,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_2
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                2,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_3
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                3,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_4
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                4,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_5
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                5,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_6
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                6,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_7
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                7,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_8
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                8,
                Transform::Displacement::UP);
        if (key == GLFW_KEY_9
            && (action == GLFW_PRESS
                || action == GLFW_REPEAT))
            Renderer::get().rotateModelJoint(0,
                9,
                Transform::Displacement::UP);
    }

    // animations
    if (key == GLFW_KEY_R
        && action == GLFW_PRESS)
        Renderer::get().toggleAnimations();

    // toggle textures
    if (key == GLFW_KEY_X
        && action == GLFW_PRESS)
        Renderer::get().toggleTextures();

    // toggle lights
    if (key == GLFW_KEY_Z
        && action == GLFW_PRESS)
        Renderer::get().toggleLights();

    // toggle shadows
    if (key == GLFW_KEY_B
        && action == GLFW_PRESS)
        Renderer::get().toggleShadows();

    // toggle axes and grid
    if (key == GLFW_KEY_C
        && action == GLFW_PRESS)
        Renderer::get().toggleFrame();

    // toggle debugging
    if (key == GLFW_KEY_V
        && action == GLFW_PRESS)
        Renderer::get().toggleDebugging();

    // toggle pathing
    if (key == GLFW_KEY_H
        && action == GLFW_PRESS)
        Renderer::get().togglePathing();

    // toggle day-night cycle
    if (key == GLFW_KEY_F
        && action == GLFW_PRESS)
        Renderer::get().toggleDayNightCycle();
}

void InputManager::processScroll(GLFWwindow* window,
    double offX,
    double offY) const {
    // currently unused
}