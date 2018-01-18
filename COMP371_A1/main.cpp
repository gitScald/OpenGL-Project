// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stb_image
#include <stb_image/stb_image.h>

// C++ standard library headers
#include <iostream>
#include <string>

// constants
const GLuint GL_VERSION_MAJOR = 3;
const GLuint GL_VERSION_MINOR = 3;
const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 800;
const GLfloat ASPECT_RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;
const std::string WINDOW_TITLE = "COMP 371 Assignment 1";

void process_input(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void exit() {
    glfwTerminate();
}

int main(int argc, char* argv[]) {
    // initialize GLFW and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        exit();

        return 1;
    }
    glfwMakeContextCurrent(window);

    // initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
    }

    // main loop
    while (!glfwWindowShouldClose(window)) {
        // process user input
        process_input(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}