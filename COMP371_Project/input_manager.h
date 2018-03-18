#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

// project headers
#include "camera.h"
#include "renderer.h"

// GLFW
#include <GLFW/glfw3.h>

// singleton
class InputManager {
public:
    InputManager(InputManager& manager) = delete;
    InputManager(InputManager&& manager) = delete;
    InputManager& operator=(InputManager& manager) = delete;
    static InputManager& get();

    void processCursor(GLFWwindow* window,
        double posX,
        double posY) const;
    void processKeyboard(GLFWwindow* window,
        int key,
        int scanCode,
        int action,
        int mods) const;
    void processScroll(GLFWwindow* window,
        double offX,
        double offY) const;

private:
    InputManager() {}

    static InputManager& s_instance;
};

#endif // !INPUT_MANAGER_H