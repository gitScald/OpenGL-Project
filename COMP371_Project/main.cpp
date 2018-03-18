// project headers
#include "camera.h"
#include "constants.h"
#include "renderer.h"
#include "input_manager.h"

// GLEW
#include <gl/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// C++ standard library headers
#include <stdlib.h>

void callbackCursorPos(GLFWwindow* window,
    double posX,
    double posY) {
    // cursor callback
    InputManager::get().processCursor(window, posX, posY);
}

void callbackKeyboard(GLFWwindow* window,
    int key,
    int scanCode,
    int action,
    int mods) {
    // keyboard callback
    InputManager::get().processKeyboard(window, key, scanCode, action, mods);
}

void callbackScroll(GLFWwindow* window,
    double offX,
    double offY) {
    // scroll callback
    InputManager::get().processScroll(window, offX, offY);
}

void callbackWindowResize(GLFWwindow* window,
    int width,
    int height) {
    // adjust viewport when window is resized
    glViewport(0, 0, width, height);
    Camera::get().setViewportDimensions(width, height);
}

int main(int argc, char* argv[]) {
    // initialize GLFW and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GL_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH,
        SCREEN_HEIGHT,
        WINDOW_TITLE.c_str(),
        NULL,
        NULL);
    if (!window) {
        std::cerr << ">>> Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return 1;
    }
    glfwMakeContextCurrent(window);

    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        std::cerr << ">>> Failed to initialize GLEW" << std::endl;

    // get current OpenGL version
    const GLubyte* STR_RENDERER = glGetString(GL_RENDERER);
    const GLubyte* STR_VERSION = glGetString(GL_VERSION);
    std::cout << "Renderer: " << STR_RENDERER << std::endl
        << "OpenGL version: " << STR_VERSION << std::endl
        << std::endl;

    // lock buffer swapping to screen refresh rate
    glfwSwapInterval(1);

    // set initial viewport size
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // enable back face culling
    glCullFace(GL_BACK);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // enable blending for transparency support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // set up callbacks
    glfwSetFramebufferSizeCallback(window, callbackWindowResize);
    glfwSetKeyCallback(window, callbackKeyboard);
    glfwSetCursorPosCallback(window, callbackCursorPos);
    glfwSetScrollCallback(window, callbackScroll);

    // delta time
    GLfloat deltaTime{ 0.0f };
    GLfloat frameCurrent{ 0.0f };
    GLfloat frameLast{ 0.0f };

    // initialize rand seed
    srand(static_cast<GLuint>(glfwGetTime()));

    // main loop
    while (!glfwWindowShouldClose(window)) {
        // get delta time since last frame
        frameCurrent = static_cast<GLfloat>(glfwGetTime());
        deltaTime = frameCurrent - frameLast;
        frameLast = frameCurrent;
        
        // adjust camera move speed
        Camera::get().setSpeedCurrent(Camera::get().getSpeed()
            * deltaTime);

        // render
        Renderer::get().render(deltaTime);

        // swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate GLFW
    glfwTerminate();

    return 0;
}