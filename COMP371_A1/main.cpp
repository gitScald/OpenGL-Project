// project headers
#include "camera.h"
#include "globals.h"
#include "renderer.h"

// GLEW
#include <gl/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// stb_image (texture loading -- not yet used)
#include <stb_image/stb_image.h>

void callback_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    GLfloat xpos_ = static_cast<GLfloat>(xpos);
    GLfloat ypos_ = static_cast<GLfloat>(ypos);
    GLfloat xoff = xpos_ - Camera::get().x();
    GLfloat yoff = Camera::get().y() - ypos_;

    // synchronize camera with initial cursor position
    if (!Camera::get().synced()) {
        Camera::get().x(xpos_);
        Camera::get().y(ypos_);
        Camera::get().synced(false);
    }

    // update camera x and y
    Camera::get().x(xpos_);
    Camera::get().y(ypos_);

    // update camera angles
    xoff *= Camera::get().sensitivity();
    yoff *= Camera::get().sensitivity();
    Camera::get().yaw(Camera::get().yaw() + xoff);
    Camera::get().pitch(Camera::get().pitch() + yoff);
    Camera::get().clamp_pitch();

    // update camera vectors
    Camera::get().update_vectors();
}

void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // process user keyboard input
    if (action == GLFW_PRESS) {
        // close window
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "Closing window..." << std::endl;
        }
    }
}

void callback_scroll(GLFWwindow* window, double xoff, double yoff) {
    // adjust camera zoom
    GLfloat yoff_ = static_cast<GLfloat>(yoff);
    Camera::get().fov(Camera::get().fov() - yoff_);
    Camera::get().clamp_fov();
}

void callback_window_resize(GLFWwindow* window, int width, int height) {
    // adjust viewport when window is resized
    glViewport(0, 0, width, height);
}

void import_vertex_data(const std::string& path_data, GLfloat* vertices) {
    // read vertex data from an external file and put it in vertices
    //std::ifstream ifs_data;
    //std::stringstream ss_data;
    //std::string line_data;
    //std::string token_data;

    //// enable ifstream exceptions to be thrown
    //ifs_data.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    //// read vertex data file
    //try {
    //    ifs_data.open(path_data);

    //    GLuint i{ 0 };

    //    std::getline(ifs_data, line_data);
    //    while (!line_data.empty()) {
    //        ss_data.clear();
    //        ss_data << line_data;
    //        std::getline(ss_data, token_data, ',');
    //        while (!token_data.empty()) {
    //            GLfloat token = std::stof(token_data);
    //            std::getline(ss_data, token_data, ',');
    //            std::cout << token << ", ";
    //        }
    //    }

    //    ifs_data.close();
    //}
    //catch (std::ifstream::failure e) {
    //    std::cout << "Failed to establish input stream with vertex data file: " << path_data << std::endl << e.what() << std::endl;
    //}
}

void process_input(GLFWwindow* window, Renderer* renderer) {
    // process continuous user input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Camera::get().move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Camera::get().move(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Camera::get().move(LEFTWARD);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Camera::get().move(RIGHTWARD);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Camera::get().move(UPWARD);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        Camera::get().move(DOWNWARD);

    // change rendering primitive
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && renderer->primitive() != POINTS) {
        renderer->primitive(POINTS);
        std::cout << "Rendering primitive changed to: POINTS" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && renderer->primitive() != LINES) {
        renderer->primitive(LINES);
        std::cout << "Rendering primitive changed to: LINES" << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && renderer->primitive() != TRIANGLES) {
        renderer->primitive(TRIANGLES);
        std::cout << "Rendering primitive changed to: TRIANGLES" << std::endl;
    }
}

// -------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    // initialize GLFW and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GL_DEPTH_BITS, 24);
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE.c_str(), NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return 1;
    }
    glfwMakeContextCurrent(window);

    // initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        std::cout << "Failed to initialize GLEW" << std::endl;

    // get current OpenGL version
    const GLubyte* STR_RENDERER = glGetString(GL_RENDERER);
    const GLubyte* STR_VERSION = glGetString(GL_VERSION);
    std::cout << "Renderer: " << STR_RENDERER << std::endl << "OpenGL version: " << STR_VERSION << std::endl;

    // lock buffer swapping to screen refresh rate
    glfwSwapInterval(1);

    // enable back face culling
    glCullFace(GL_BACK);

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // enable blending for transparency support
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // enable cursor capture
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set up callbacks
    glfwSetFramebufferSizeCallback(window, callback_window_resize);
    glfwSetKeyCallback(window, callback_keyboard);
    glfwSetCursorPosCallback(window, callback_cursor_pos);
    glfwSetScrollCallback(window, callback_scroll);

    // delta time
    GLfloat delta_time{ 0.0f };
    GLfloat frame_curr{ 0.0f };
    GLfloat frame_last{ 0.0f };

    // test
    //GLfloat test[1212];
    //import_vertex_data("data/grid/vertex.csv", test);

    // renderer
    Renderer renderer;

    // main loop
    while (!glfwWindowShouldClose(window)) {
        // get delta time since last frame and adjust camera move speed
        frame_curr = static_cast<GLfloat>(glfwGetTime());
        delta_time = frame_curr - frame_last;
        frame_last = frame_curr;
        Camera::get().speed_curr(Camera::get().speed_base() * delta_time);

        // process movement input
        process_input(window, &renderer);

        // render
        renderer.render();

        // swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate GLFW
    glfwTerminate();

    return 0;
}