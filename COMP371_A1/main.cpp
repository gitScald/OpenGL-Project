// GLEW     #define GLEW_STATIC
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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// constants
const GLuint GL_VERSION_MAJOR = 3;
const GLuint GL_VERSION_MINOR = 3;
const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 800;
const GLfloat ASPECT_RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;
const std::string WINDOW_TITLE = "COMP 371 Assignment 1";
const GLfloat Z_NEAR = 0.1f;
const GLfloat Z_FAR = 100.0f;
const GLfloat MAX_PITCH = 89.0f;
const GLfloat MAX_FOV = 45.0f;
const GLfloat MIN_FOV = 1.0f;

// rendering primitives
const enum RENDERING_PRIMITIVE {
    POINTS,
    LINES,
    TRIANGLES
};
RENDERING_PRIMITIVE current_primitive = TRIANGLES;

// -------------------------------------------------------------------------------------

// camera movement values
const enum CAMERA_MOVEMENT {
    FORWARD,
    BACKWARD,
    LEFTWARD,
    RIGHTWARD,
    UPWARD,
    DOWNWARD
};

class Camera {
public:
    Camera(const glm::vec3& pos, const glm::vec3& up_cam, const glm::vec3& frnt = glm::vec3(0.0f, 0.0f, -1.0f), const glm::vec3& up_wrld = glm::vec3(0.0f, 1.0f, 0.0f))
        : position{ pos }, up_camera{ up_cam }, front{ frnt }, up_world{ up_wrld } {
        update_vectors();
    }

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
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up_camera;
    glm::vec3 up_world;
    glm::mat4 projection;
    glm::mat4 view;
    GLfloat mouse_x{ SCREEN_WIDTH / 2.0f };
    GLfloat mouse_y{ SCREEN_HEIGHT / 2.0f };
    GLfloat mouse_yaw{ -180.0f };
    GLfloat mouse_pitch{ 0.0f };
    GLfloat move_speed_base{ 2.5f };
    GLfloat move_speed_curr{ move_speed_base };
    GLfloat mouse_sensitivity{ 0.1f };
    GLfloat field_of_view{ MAX_FOV };
    bool is_synced{ false };
};

void Camera::clamp_pitch() {
    // avoid the camera from looking upside down
    if (mouse_pitch < -89.0f)
        mouse_pitch = 89.0f;
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
        position += up_camera * move_speed_curr;
        break;
    case DOWNWARD:
        position -= up_camera * move_speed_curr;
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

// camera object
Camera camera(glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

// -------------------------------------------------------------------------------------

class Shader {
public:
    Shader(const std::string& path_vertex, const std::string& path_fragment);
    ~Shader();

    void set_mat4(const std::string& uniform, const glm::mat4& value) const;
    void use() const;

private:
    void compile_shader(const std::string& shdr_type, GLuint shdr_id) const;
    void link_program(GLuint shdr_vertex, GLuint shdr_fragment) const;

    GLuint id;
};

Shader::Shader(const std::string& path_vertex, const std::string& path_fragment) {
    std::ifstream ifs_vertex, ifs_fragment;
    std::stringstream ss_vertex, ss_fragment;
    std::string code_vertex, code_fragment;

    // enable ifstream exceptions to be thrown
    ifs_vertex.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    ifs_fragment.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    // read shader code
    try {
        ifs_vertex.open(path_vertex);
        ss_vertex << ifs_vertex.rdbuf();
        code_vertex = ss_vertex.str();
        ifs_vertex.close();

        ifs_fragment.open(path_fragment);
        ss_fragment << ifs_fragment.rdbuf();
        code_fragment = ss_fragment.str();
        ifs_fragment.close();
    }
    catch (std::ifstream::failure e) {
        std::cout << "Failed to establish input stream with shader file" << std::endl << e.what() << std::endl;
    }
    const GLchar* src_vertex = code_vertex.c_str();
    const GLchar* src_fragment = code_fragment.c_str();

    // id's for the two shaders
    GLuint shdr_vertex, shdr_fragment;

    // create and compile vertex shader
    shdr_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shdr_vertex, 1, &src_vertex, NULL);
    compile_shader("GL_VERTEX_SHADER", shdr_vertex);

    // create and compile fragment shader
    shdr_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shdr_fragment, 1, &src_fragment, NULL);
    compile_shader("GL_FRAGMENT_SHADER", shdr_fragment);

    // create and link shader program
    id = glCreateProgram();
    glAttachShader(id, shdr_vertex);
    glAttachShader(id, shdr_fragment);
    glLinkProgram(id);

    // free up memory from the two shaders
    glDeleteShader(shdr_vertex);
    glDeleteShader(shdr_fragment);
}

Shader::~Shader() {
    // free up memory from the shader program
    glDeleteProgram(id);
}

void Shader::set_mat4(const std::string& uniform, const glm::mat4& value) const {
    // sets a mat4 uniform
    glUniformMatrix4fv(glGetUniformLocation(id, uniform.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::compile_shader(const std::string& shdr_type, GLuint shdr_id) const {
    // attempt to compile shader and report any issues
    glCompileShader(shdr_id);

    GLint status;
    glGetShaderiv(shdr_id, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLchar log[1024];
        glGetShaderInfoLog(shdr_id, 1024, NULL, log);
        std::cout << "Failed to compile shader: " << shdr_type << std::endl << log << std::endl;
    }
    else
        std::cout << "Shader compilation successful: " << shdr_type << std::endl;
}

void Shader::link_program(GLuint shdr_vertex, GLuint shdr_fragment) const {
    // attempt to link program and report any issues
    glAttachShader(id, shdr_vertex);
    glAttachShader(id, shdr_fragment);
    glLinkProgram(id);

    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (!status) {
        GLchar log[1024];
        glGetProgramInfoLog(id, 1024, NULL, log);
        std::cout << "Failed to link program" << std::endl << log << std::endl;
    }
    else
        std::cout << "Shader program linking successful" << std::endl;
}

// -------------------------------------------------------------------------------------

void callback_cursor_pos(GLFWwindow* window, double xpos, double ypos) {
    GLfloat xpos_ = static_cast<GLfloat>(xpos);
    GLfloat ypos_ = static_cast<GLfloat>(ypos);
    GLfloat xoff = xpos_ - camera.x();
    GLfloat yoff = camera.y() - ypos_;

    // synchronize camera with initial cursor position
    if (!camera.synced()) {
        camera.x(xpos_);
        camera.y(ypos_);
        camera.synced(false);
    }

    // update camera x and y
    camera.x(xpos_);
    camera.y(ypos_);

    // update camera angles
    xoff *= camera.sensitivity();
    yoff *= camera.sensitivity();
    camera.yaw(camera.yaw() + xoff);
    camera.pitch(camera.pitch() + yoff);
    camera.clamp_pitch();

    // update camera vectors
    camera.update_vectors();
}

void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // process user keyboard input
    if (action == GLFW_PRESS) {
        // close window
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "Closing window..." << std::endl;
        }

        // change rendering primitive
        if (key == GLFW_KEY_P && current_primitive != POINTS) {
            current_primitive = POINTS;
            std::cout << "Rendering primitive changed to: POINTS" << std::endl;
        }
        if (key == GLFW_KEY_L && current_primitive != LINES) {
            current_primitive = LINES;
            std::cout << "Rendering primitive changed to: LINES" << std::endl;
        }
        if (key == GLFW_KEY_T && current_primitive != TRIANGLES) {
            current_primitive = TRIANGLES;
            std::cout << "Rendering primitive changed to: TRIANGLES" << std::endl;
        }
    }
}

void callback_scroll(GLFWwindow* window, double xoff, double yoff) {
    // adjust camera zoom
    GLfloat yoff_ = static_cast<GLfloat>(yoff);
    camera.fov(camera.fov() - yoff_);    
    camera.clamp_fov();
}

void callback_window_resize(GLFWwindow* window, int width, int height) {
    // adjust viewport when window is resized
    glViewport(0, 0, width, height);
}

// -------------------------------------------------------------------------------------

void process_input(GLFWwindow* window) {
    // process continuous user input
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.move(FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.move(BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.move(LEFTWARD);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.move(RIGHTWARD);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.move(UPWARD);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.move(DOWNWARD);
}

// -------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    // initialize GLFW and create window
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

    // enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // enable cursor capture
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // set up callbacks
    glfwSetFramebufferSizeCallback(window, callback_window_resize);
    glfwSetKeyCallback(window, callback_keyboard);
    glfwSetCursorPosCallback(window, callback_cursor_pos);
    glfwSetScrollCallback(window, callback_scroll);
    
    // vertex positions for the cube
    GLfloat vertices[] = {
        // front face
        -0.5f,  0.5f,  0.5f,    // top left
        -0.5f, -0.5f,  0.5f,    // bottom left
         0.5f, -0.5f,  0.5f,    // bottom right
         0.5f,  0.5f,  0.5f,    // top right

         // back face
        -0.5f,  0.5f, -0.5f,    // top left
        -0.5f, -0.5f, -0.5f,    // bottom left
         0.5f, -0.5f, -0.5f,    // bottom right
         0.5f,  0.5f, -0.5f,    // top right

         // left face
        -0.5f,  0.5f, -0.5f,    // top left
        -0.5f, -0.5f, -0.5f,    // bottom left
        -0.5f, -0.5f,  0.5f,    // bottom right
        -0.5f,  0.5f,  0.5f,    // top right

         // right face
         0.5f,  0.5f, -0.5f,    // top left
         0.5f, -0.5f, -0.5f,    // bottom left
         0.5f, -0.5f,  0.0f,    // bottom right
         0.5f,  0.5f,  0.0f,    // top right

         // bottom face
        -0.5f, -0.5f, -0.5f,    // top left
        -0.5f, -0.5f,  0.5f,    // bottom left
         0.5f, -0.5f,  0.5f,    // bottom right
         0.5f, -0.5f, -0.5f,    // top right

         // top face
        -0.5f,  0.5f, -0.5f,    // top left
        -0.5f,  0.5f,  0.5f,    // bottom left
         0.5f,  0.5f,  0.5f,    // bottom right
         0.5f,  0.5f, -0.5f,    // top right
    };

    // indices for the element array buffer
    GLuint indices[] = {
        // front face
         0,  1,  2,     // first triangle
         0,  2,  3,     // second triangle

        // back face
         4,  5,  6,     // first triangle
         4,  6,  7,     // second triangle

        // left face
         8,  9, 10,     // first triangle
         8, 10, 11,     // second triangle

        // right face
        12, 13, 14,     // first triangle
        12, 14, 15,     // second triangle

        // bottom face
        16, 17, 18,     // first triangle
        16, 18, 19,     // second triangle

        // top face
        20, 21, 22,     // first triangle
        20, 22, 23      // second triangle
    };

    // id's for the various objects
    GLuint VAO, VBO, EBO;

    // generate and bind vertex array object, buffer data
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // generate and bind vertex buffect object
    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // generate and bind element array buffer
    glCreateBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // unbind vertex array object
    glBindVertexArray(VAO);

    // initialize model matrix and cube position
    glm::mat4 model_matrix;
    glm::vec3 cube_position{ glm::vec3(0.0f, 0.0f, 0.0f) };

    // initialize shader program
    const std::string PATH_VERTEX{ "shaders/vertex.shdr" };
    const std::string PATH_FRAGMENT{ "shaders/fragment.shdr" };
    Shader shader(PATH_VERTEX, PATH_FRAGMENT);

    // delta time
    GLfloat delta_time{ 0.0f };
    GLfloat frame_curr{ 0.0f };
    GLfloat frame_last{ 0.0f };

    // main loop
    while (!glfwWindowShouldClose(window)) {
        // get delta time since last frame and adjust camera move speed
        frame_curr = static_cast<GLfloat>(glfwGetTime());
        delta_time = frame_curr - frame_last;
        frame_last = frame_curr;
        camera.speed_curr(camera.speed_base() * delta_time);

        // process movement input
        process_input(window);

        // clear screen and activate shader
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();

        // set shader uniforms (reset model matrix between each cube render)
        model_matrix = glm::mat4();
        model_matrix = glm::translate(model_matrix, cube_position);
        shader.set_mat4("model", model_matrix);
        shader.set_mat4("view", camera.view_matrix());
        shader.set_mat4("projection", camera.projection_matrix());

        // render cube using current primitive
        glBindVertexArray(VAO);
        switch (current_primitive) {
        case POINTS:
            glDrawElements(GL_POINTS, 36, GL_UNSIGNED_INT, 0);
            break;
        case LINES:
            glDrawElements(GL_LINES, 36, GL_UNSIGNED_INT, 0);
            break;
        case TRIANGLES:
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            break;
        }
        glBindVertexArray(NULL);

        // swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // free up memory and terminate GLFW
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();

    return 0;
}