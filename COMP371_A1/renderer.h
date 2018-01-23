#ifndef RENDERER_H
#define RENDERER_H

// project headers
#include "camera.h"
#include "globals.h"
#include "shader.h"

// GLEW
#include <gl/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// rendering primitives
const enum RENDERING_PRIMITIVE {
    POINTS,
    LINES,
    TRIANGLES
};

class Renderer {
public:
    Renderer();
    Renderer(Renderer& rndr) = delete;
    ~Renderer();
    Renderer& operator=(Renderer const& rndr) = delete;

    RENDERING_PRIMITIVE primitive() const;
    void primitive(RENDERING_PRIMITIVE new_primitive);
    void render();

private:
    void init_cube();
    void init_axis();
    void init_grid();
    void init_properties();

    RENDERING_PRIMITIVE current_primitive{ TRIANGLES };
    GLuint VAO_cube;
    GLuint VAO_axis;
    GLuint VAO_grid;
    GLuint VBO_cube;
    GLuint VBO_axis;
    GLuint VBO_grid;
    GLuint EBO_cube;
    Shader shader_cube{ PATH_VERTEX_CUBE, PATH_FRAGMENT_CUBE };
    Shader shader_axis{ PATH_VERTEX_AXIS, PATH_FRAGMENT_AXIS };
    Shader shader_grid{ PATH_VERTEX_GRID, PATH_FRAGMENT_GRID };
    glm::mat4 model_matrix;
    glm::vec3 position_cube{ glm::vec3(0.0f, 0.5f, 0.0f) };
    glm::vec3 position_axis{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 position_grid{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec4 axis_colors[3];
    std::pair<glm::vec3, GLfloat> axis_rotations[3];
    GLfloat delta_time{ 0.0f };
    GLfloat frame_curr{ 0.0f };
    GLfloat frame_last{ 0.0f };
};

#endif // !RENDERER_H