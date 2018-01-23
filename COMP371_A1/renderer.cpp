#include "renderer.h"

Renderer::Renderer() {
    init_cube();
    init_axis();
    init_grid();
    init_properties();

    // unbind vertex array object
    glBindVertexArray(NULL);
}

Renderer::~Renderer() {
    // free up memory
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteVertexArrays(1, &VAO_axis);
    glDeleteVertexArrays(1, &VAO_grid);
    glDeleteBuffers(1, &VBO_cube);
    glDeleteBuffers(1, &VBO_axis);
    glDeleteBuffers(1, &VBO_grid);
    glDeleteBuffers(1, &EBO_cube);
}

RENDERING_PRIMITIVE Renderer::primitive() const {
    // return the current rendering primitive
    return current_primitive;
}

void Renderer::primitive(RENDERING_PRIMITIVE new_primitive) {
    // set the rendering primitive
    current_primitive = new_primitive;
}

void Renderer::render() {
    // clear screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set cube shader uniforms
    shader_cube.use();
    model_matrix = glm::mat4();
    model_matrix = glm::translate(model_matrix, position_cube);
    shader_cube.set_mat4("model", model_matrix);
    shader_cube.set_mat4("view", Camera::get().view_matrix());
    shader_cube.set_mat4("projection", Camera::get().projection_matrix());

    // render cube using current primitive
    glBindVertexArray(VAO_cube);
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

    // set axis shader uniforms
    shader_axis.use();
    shader_axis.set_mat4("view", Camera::get().view_matrix());
    shader_axis.set_mat4("projection", Camera::get().projection_matrix());

    // render axes
    glBindVertexArray(VAO_axis);
    for (GLuint i = 0; i != 3; ++i) {
        model_matrix = glm::mat4();
        model_matrix = glm::translate(model_matrix, position_axis);
        model_matrix = glm::rotate(model_matrix, glm::radians(axis_rotations[i].second), axis_rotations[i].first);
        shader_axis.set_mat4("model", model_matrix);
        shader_axis.set_vec4("axis_color", axis_colors[i]);
        glDrawArrays(GL_LINES, 0, 2);
    }

    // set grid shader uniforms
    shader_grid.use();
    model_matrix = glm::mat4();
    model_matrix = glm::translate(model_matrix, position_grid);
    shader_grid.set_mat4("model", model_matrix);
    shader_grid.set_mat4("view", Camera::get().view_matrix());
    shader_grid.set_mat4("projection", Camera::get().projection_matrix());
    glDrawArrays(GL_LINES, 0, 2);

    // render grid
    glBindVertexArray(VAO_grid);
    glDrawArrays(GL_LINES, 0, 404);
}

void Renderer::init_cube() {
    // vertex positions for the cube
    GLfloat vertices_cube[] = {
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
    GLuint indices_cube[] = {
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

    // generate and bind vertex array object
    glCreateVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);

    // generate and bind vertex buffect object, buffer data
    glCreateBuffers(1, &VBO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_cube), vertices_cube, GL_STATIC_DRAW);

    // generate and bind element array buffer, buffer data
    glCreateBuffers(1, &EBO_cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_cube);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_cube), indices_cube, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::init_axis() {
    // vertex positions for the lines representing the axes
    GLfloat vertices_axis[] = {
        // position
        0.0f, 0.0f, 0.0f,   // line origin
        5.0f, 0.0f, 0.0f    // line end
    };

    // generate and bind vertex array object
    glCreateVertexArrays(1, &VAO_axis);
    glBindVertexArray(VAO_axis);

    // generate and bind vertex buffer object, buffer data
    glCreateBuffers(1, &VBO_axis);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_axis);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axis), vertices_axis, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::init_grid() {
    // vertex positions for the lines representing the grid squares
    GLfloat vertices_grid[] = {
        // horizontal lines
        -50.0f, 0.0f, -50.0f,
         50.0f, 0.0f, -50.0f,
        -50.0f, 0.0f, -49.0f,
         50.0f, 0.0f, -49.0f,
        -50.0f, 0.0f, -48.0f,
         50.0f, 0.0f, -48.0f,
        -50.0f, 0.0f, -47.0f,
         50.0f, 0.0f, -47.0f,
        -50.0f, 0.0f, -46.0f,
         50.0f, 0.0f, -46.0f,
        -50.0f, 0.0f, -45.0f,
         50.0f, 0.0f, -45.0f,
        -50.0f, 0.0f, -44.0f,
         50.0f, 0.0f, -44.0f,
        -50.0f, 0.0f, -43.0f,
         50.0f, 0.0f, -43.0f,
        -50.0f, 0.0f, -42.0f,
         50.0f, 0.0f, -42.0f,
        -50.0f, 0.0f, -41.0f,
         50.0f, 0.0f, -41.0f,
        -50.0f, 0.0f, -40.0f,
         50.0f, 0.0f, -40.0f,
        -50.0f, 0.0f, -39.0f,
         50.0f, 0.0f, -39.0f,
        -50.0f, 0.0f, -38.0f,
         50.0f, 0.0f, -38.0f,
        -50.0f, 0.0f, -37.0f,
         50.0f, 0.0f, -37.0f,
        -50.0f, 0.0f, -36.0f,
         50.0f, 0.0f, -36.0f,
        -50.0f, 0.0f, -35.0f,
         50.0f, 0.0f, -35.0f,
        -50.0f, 0.0f, -34.0f,
         50.0f, 0.0f, -34.0f,
        -50.0f, 0.0f, -33.0f,
         50.0f, 0.0f, -33.0f,
        -50.0f, 0.0f, -32.0f,
         50.0f, 0.0f, -32.0f,
        -50.0f, 0.0f, -31.0f,
         50.0f, 0.0f, -31.0f,
        -50.0f, 0.0f, -30.0f,
         50.0f, 0.0f, -30.0f,
        -50.0f, 0.0f, -29.0f,
         50.0f, 0.0f, -29.0f,
        -50.0f, 0.0f, -28.0f,
         50.0f, 0.0f, -28.0f,
        -50.0f, 0.0f, -27.0f,
         50.0f, 0.0f, -27.0f,
        -50.0f, 0.0f, -26.0f,
         50.0f, 0.0f, -26.0f,
        -50.0f, 0.0f, -25.0f,
         50.0f, 0.0f, -25.0f,
        -50.0f, 0.0f, -24.0f,
         50.0f, 0.0f, -24.0f,
        -50.0f, 0.0f, -23.0f,
         50.0f, 0.0f, -23.0f,
        -50.0f, 0.0f, -22.0f,
         50.0f, 0.0f, -22.0f,
        -50.0f, 0.0f, -21.0f,
         50.0f, 0.0f, -21.0f,
        -50.0f, 0.0f, -20.0f,
         50.0f, 0.0f, -20.0f,
        -50.0f, 0.0f, -19.0f,
         50.0f, 0.0f, -19.0f,
        -50.0f, 0.0f, -18.0f,
         50.0f, 0.0f, -18.0f,
        -50.0f, 0.0f, -17.0f,
         50.0f, 0.0f, -17.0f,
        -50.0f, 0.0f, -16.0f,
         50.0f, 0.0f, -16.0f,
        -50.0f, 0.0f, -15.0f,
         50.0f, 0.0f, -15.0f,
        -50.0f, 0.0f, -14.0f,
         50.0f, 0.0f, -14.0f,
        -50.0f, 0.0f, -13.0f,
         50.0f, 0.0f, -13.0f,
        -50.0f, 0.0f, -12.0f,
         50.0f, 0.0f, -12.0f,
        -50.0f, 0.0f, -11.0f,
         50.0f, 0.0f, -11.0f,
        -50.0f, 0.0f, -10.0f,
         50.0f, 0.0f, -10.0f,
        -50.0f, 0.0f, -9.0f,
         50.0f, 0.0f, -9.0f,
        -50.0f, 0.0f, -8.0f,
         50.0f, 0.0f, -8.0f,
        -50.0f, 0.0f, -7.0f,
         50.0f, 0.0f, -7.0f,
        -50.0f, 0.0f, -6.0f,
         50.0f, 0.0f, -6.0f,
        -50.0f, 0.0f, -5.0f,
         50.0f, 0.0f, -5.0f,
        -50.0f, 0.0f, -4.0f,
         50.0f, 0.0f, -4.0f,
        -50.0f, 0.0f, -3.0f,
         50.0f, 0.0f, -3.0f,
        -50.0f, 0.0f, -2.0f,
         50.0f, 0.0f, -2.0f,
        -50.0f, 0.0f, -1.0f,
         50.0f, 0.0f, -1.0f,
        -50.0f, 0.0f,  0.0f,
         50.0f, 0.0f,  0.0f,
        -50.0f, 0.0f,  1.0f,
         50.0f, 0.0f,  1.0f,
        -50.0f, 0.0f,  2.0f,
         50.0f, 0.0f,  2.0f,
        -50.0f, 0.0f,  3.0f,
         50.0f, 0.0f,  3.0f,
        -50.0f, 0.0f,  4.0f,
         50.0f, 0.0f,  4.0f,
        -50.0f, 0.0f,  5.0f,
         50.0f, 0.0f,  5.0f,
        -50.0f, 0.0f,  6.0f,
         50.0f, 0.0f,  6.0f,
        -50.0f, 0.0f,  7.0f,
         50.0f, 0.0f,  7.0f,
        -50.0f, 0.0f,  8.0f,
         50.0f, 0.0f,  8.0f,
        -50.0f, 0.0f,  9.0f,
         50.0f, 0.0f,  9.0f,
        -50.0f, 0.0f, 10.0f,
         50.0f, 0.0f, 10.0f,
        -50.0f, 0.0f, 11.0f,
         50.0f, 0.0f, 11.0f,
        -50.0f, 0.0f, 12.0f,
         50.0f, 0.0f, 12.0f,
        -50.0f, 0.0f, 13.0f,
         50.0f, 0.0f, 13.0f,
        -50.0f, 0.0f, 14.0f,
         50.0f, 0.0f, 14.0f,
        -50.0f, 0.0f, 15.0f,
         50.0f, 0.0f, 15.0f,
        -50.0f, 0.0f, 16.0f,
         50.0f, 0.0f, 16.0f,
        -50.0f, 0.0f, 17.0f,
         50.0f, 0.0f, 17.0f,
        -50.0f, 0.0f, 18.0f,
         50.0f, 0.0f, 18.0f,
        -50.0f, 0.0f, 19.0f,
         50.0f, 0.0f, 19.0f,
        -50.0f, 0.0f, 20.0f,
         50.0f, 0.0f, 20.0f,
        -50.0f, 0.0f, 21.0f,
         50.0f, 0.0f, 21.0f,
        -50.0f, 0.0f, 22.0f,
         50.0f, 0.0f, 22.0f,
        -50.0f, 0.0f, 23.0f,
         50.0f, 0.0f, 23.0f,
        -50.0f, 0.0f, 24.0f,
         50.0f, 0.0f, 24.0f,
        -50.0f, 0.0f, 25.0f,
         50.0f, 0.0f, 25.0f,
        -50.0f, 0.0f, 26.0f,
         50.0f, 0.0f, 26.0f,
        -50.0f, 0.0f, 27.0f,
         50.0f, 0.0f, 27.0f,
        -50.0f, 0.0f, 28.0f,
         50.0f, 0.0f, 28.0f,
        -50.0f, 0.0f, 29.0f,
         50.0f, 0.0f, 29.0f,
        -50.0f, 0.0f, 30.0f,
         50.0f, 0.0f, 30.0f,
        -50.0f, 0.0f, 31.0f,
         50.0f, 0.0f, 31.0f,
        -50.0f, 0.0f, 32.0f,
         50.0f, 0.0f, 32.0f,
        -50.0f, 0.0f, 33.0f,
         50.0f, 0.0f, 33.0f,
        -50.0f, 0.0f, 34.0f,
         50.0f, 0.0f, 34.0f,
        -50.0f, 0.0f, 35.0f,
         50.0f, 0.0f, 35.0f,
        -50.0f, 0.0f, 36.0f,
         50.0f, 0.0f, 36.0f,
        -50.0f, 0.0f, 37.0f,
         50.0f, 0.0f, 37.0f,
        -50.0f, 0.0f, 38.0f,
         50.0f, 0.0f, 38.0f,
        -50.0f, 0.0f, 39.0f,
         50.0f, 0.0f, 39.0f,
        -50.0f, 0.0f, 40.0f,
         50.0f, 0.0f, 40.0f,
        -50.0f, 0.0f, 41.0f,
         50.0f, 0.0f, 41.0f,
        -50.0f, 0.0f, 42.0f,
         50.0f, 0.0f, 42.0f,
        -50.0f, 0.0f, 43.0f,
         50.0f, 0.0f, 43.0f,
        -50.0f, 0.0f, 44.0f,
         50.0f, 0.0f, 44.0f,
        -50.0f, 0.0f, 45.0f,
         50.0f, 0.0f, 45.0f,
        -50.0f, 0.0f, 46.0f,
         50.0f, 0.0f, 46.0f,
        -50.0f, 0.0f, 47.0f,
         50.0f, 0.0f, 47.0f,
        -50.0f, 0.0f, 48.0f,
         50.0f, 0.0f, 48.0f,
        -50.0f, 0.0f, 49.0f,
         50.0f, 0.0f, 49.0f,
        -50.0f, 0.0f, 50.0f,
         50.0f, 0.0f, 50.0f,

        // vertical lines
        -50.0f, 0.0f, -50.0f,
        -50.0f, 0.0f,  50.0f,
        -49.0f, 0.0f, -50.0f,
        -49.0f, 0.0f,  50.0f,
        -48.0f, 0.0f, -50.0f,
        -48.0f, 0.0f,  50.0f,
        -47.0f, 0.0f, -50.0f,
        -47.0f, 0.0f,  50.0f,
        -46.0f, 0.0f, -50.0f,
        -46.0f, 0.0f,  50.0f,
        -45.0f, 0.0f, -50.0f,
        -45.0f, 0.0f,  50.0f,
        -44.0f, 0.0f, -50.0f,
        -44.0f, 0.0f,  50.0f,
        -43.0f, 0.0f, -50.0f,
        -43.0f, 0.0f,  50.0f,
        -42.0f, 0.0f, -50.0f,
        -42.0f, 0.0f,  50.0f,
        -41.0f, 0.0f, -50.0f,
        -41.0f, 0.0f,  50.0f,
        -40.0f, 0.0f, -50.0f,
        -40.0f, 0.0f,  50.0f,
        -39.0f, 0.0f, -50.0f,
        -39.0f, 0.0f,  50.0f,
        -38.0f, 0.0f, -50.0f,
        -38.0f, 0.0f,  50.0f,
        -37.0f, 0.0f, -50.0f,
        -37.0f, 0.0f,  50.0f,
        -36.0f, 0.0f, -50.0f,
        -36.0f, 0.0f,  50.0f,
        -35.0f, 0.0f, -50.0f,
        -35.0f, 0.0f,  50.0f,
        -34.0f, 0.0f, -50.0f,
        -34.0f, 0.0f,  50.0f,
        -33.0f, 0.0f, -50.0f,
        -33.0f, 0.0f,  50.0f,
        -32.0f, 0.0f, -50.0f,
        -32.0f, 0.0f,  50.0f,
        -31.0f, 0.0f, -50.0f,
        -31.0f, 0.0f,  50.0f,
        -30.0f, 0.0f, -50.0f,
        -30.0f, 0.0f,  50.0f,
        -29.0f, 0.0f, -50.0f,
        -29.0f, 0.0f,  50.0f,
        -28.0f, 0.0f, -50.0f,
        -28.0f, 0.0f,  50.0f,
        -27.0f, 0.0f, -50.0f,
        -27.0f, 0.0f,  50.0f,
        -26.0f, 0.0f, -50.0f,
        -26.0f, 0.0f,  50.0f,
        -25.0f, 0.0f, -50.0f,
        -25.0f, 0.0f,  50.0f,
        -24.0f, 0.0f, -50.0f,
        -24.0f, 0.0f,  50.0f,
        -23.0f, 0.0f, -50.0f,
        -23.0f, 0.0f,  50.0f,
        -22.0f, 0.0f, -50.0f,
        -22.0f, 0.0f,  50.0f,
        -21.0f, 0.0f, -50.0f,
        -21.0f, 0.0f,  50.0f,
        -20.0f, 0.0f, -50.0f,
        -20.0f, 0.0f,  50.0f,
        -19.0f, 0.0f, -50.0f,
        -19.0f, 0.0f,  50.0f,
        -18.0f, 0.0f, -50.0f,
        -18.0f, 0.0f,  50.0f,
        -17.0f, 0.0f, -50.0f,
        -17.0f, 0.0f,  50.0f,
        -16.0f, 0.0f, -50.0f,
        -16.0f, 0.0f,  50.0f,
        -15.0f, 0.0f, -50.0f,
        -15.0f, 0.0f,  50.0f,
        -14.0f, 0.0f, -50.0f,
        -14.0f, 0.0f,  50.0f,
        -13.0f, 0.0f, -50.0f,
        -13.0f, 0.0f,  50.0f,
        -12.0f, 0.0f, -50.0f,
        -12.0f, 0.0f,  50.0f,
        -11.0f, 0.0f, -50.0f,
        -11.0f, 0.0f,  50.0f,
        -10.0f, 0.0f, -50.0f,
        -10.0f, 0.0f,  50.0f,
         -9.0f, 0.0f, -50.0f,
         -9.0f, 0.0f,  50.0f,
         -8.0f, 0.0f, -50.0f,
         -8.0f, 0.0f,  50.0f,
         -7.0f, 0.0f, -50.0f,
         -7.0f, 0.0f,  50.0f,
         -6.0f, 0.0f, -50.0f,
         -6.0f, 0.0f,  50.0f,
         -5.0f, 0.0f, -50.0f,
         -5.0f, 0.0f,  50.0f,
         -4.0f, 0.0f, -50.0f,
         -4.0f, 0.0f,  50.0f,
         -3.0f, 0.0f, -50.0f,
         -3.0f, 0.0f,  50.0f,
         -2.0f, 0.0f, -50.0f,
         -2.0f, 0.0f,  50.0f,
         -1.0f, 0.0f, -50.0f,
         -1.0f, 0.0f,  50.0f,
          0.0f, 0.0f, -50.0f,
          0.0f, 0.0f,  50.0f,
          1.0f, 0.0f, -50.0f,
          1.0f, 0.0f,  50.0f,
          2.0f, 0.0f, -50.0f,
          2.0f, 0.0f,  50.0f,
          3.0f, 0.0f, -50.0f,
          3.0f, 0.0f,  50.0f,
          4.0f, 0.0f, -50.0f,
          4.0f, 0.0f,  50.0f,
          5.0f, 0.0f, -50.0f,
          5.0f, 0.0f,  50.0f,
          6.0f, 0.0f, -50.0f,
          6.0f, 0.0f,  50.0f,
          7.0f, 0.0f, -50.0f,
          7.0f, 0.0f,  50.0f,
          8.0f, 0.0f, -50.0f,
          8.0f, 0.0f,  50.0f,
          9.0f, 0.0f, -50.0f,
          9.0f, 0.0f,  50.0f,
         10.0f, 0.0f, -50.0f,
         10.0f, 0.0f,  50.0f,
         11.0f, 0.0f, -50.0f,
         11.0f, 0.0f,  50.0f,
         12.0f, 0.0f, -50.0f,
         12.0f, 0.0f,  50.0f,
         13.0f, 0.0f, -50.0f,
         13.0f, 0.0f,  50.0f,
         14.0f, 0.0f, -50.0f,
         14.0f, 0.0f,  50.0f,
         15.0f, 0.0f, -50.0f,
         15.0f, 0.0f,  50.0f,
         16.0f, 0.0f, -50.0f,
         16.0f, 0.0f,  50.0f,
         17.0f, 0.0f, -50.0f,
         17.0f, 0.0f,  50.0f,
         18.0f, 0.0f, -50.0f,
         18.0f, 0.0f,  50.0f,
         19.0f, 0.0f, -50.0f,
         19.0f, 0.0f,  50.0f,
         20.0f, 0.0f, -50.0f,
         20.0f, 0.0f,  50.0f,
         21.0f, 0.0f, -50.0f,
         21.0f, 0.0f,  50.0f,
         22.0f, 0.0f, -50.0f,
         22.0f, 0.0f,  50.0f,
         23.0f, 0.0f, -50.0f,
         23.0f, 0.0f,  50.0f,
         24.0f, 0.0f, -50.0f,
         24.0f, 0.0f,  50.0f,
         25.0f, 0.0f, -50.0f,
         25.0f, 0.0f,  50.0f,
         26.0f, 0.0f, -50.0f,
         26.0f, 0.0f,  50.0f,
         27.0f, 0.0f, -50.0f,
         27.0f, 0.0f,  50.0f,
         28.0f, 0.0f, -50.0f,
         28.0f, 0.0f,  50.0f,
         29.0f, 0.0f, -50.0f,
         29.0f, 0.0f,  50.0f,
         30.0f, 0.0f, -50.0f,
         30.0f, 0.0f,  50.0f,
         31.0f, 0.0f, -50.0f,
         31.0f, 0.0f,  50.0f,
         32.0f, 0.0f, -50.0f,
         32.0f, 0.0f,  50.0f,
         33.0f, 0.0f, -50.0f,
         33.0f, 0.0f,  50.0f,
         34.0f, 0.0f, -50.0f,
         34.0f, 0.0f,  50.0f,
         35.0f, 0.0f, -50.0f,
         35.0f, 0.0f,  50.0f,
         36.0f, 0.0f, -50.0f,
         36.0f, 0.0f,  50.0f,
         37.0f, 0.0f, -50.0f,
         37.0f, 0.0f,  50.0f,
         38.0f, 0.0f, -50.0f,
         38.0f, 0.0f,  50.0f,
         39.0f, 0.0f, -50.0f,
         39.0f, 0.0f,  50.0f,
         40.0f, 0.0f, -50.0f,
         40.0f, 0.0f,  50.0f,
         41.0f, 0.0f, -50.0f,
         41.0f, 0.0f,  50.0f,
         42.0f, 0.0f, -50.0f,
         42.0f, 0.0f,  50.0f,
         43.0f, 0.0f, -50.0f,
         43.0f, 0.0f,  50.0f,
         44.0f, 0.0f, -50.0f,
         44.0f, 0.0f,  50.0f,
         45.0f, 0.0f, -50.0f,
         45.0f, 0.0f,  50.0f,
         46.0f, 0.0f, -50.0f,
         46.0f, 0.0f,  50.0f,
         47.0f, 0.0f, -50.0f,
         47.0f, 0.0f,  50.0f,
         48.0f, 0.0f, -50.0f,
         48.0f, 0.0f,  50.0f,
         49.0f, 0.0f, -50.0f,
         49.0f, 0.0f,  50.0f,
         50.0f, 0.0f, -50.0f,
         50.0f, 0.0f,  50.0f
    };

    // generate and bind vertex array object
    glCreateVertexArrays(1, &VAO_grid);
    glBindVertexArray(VAO_grid);

    // generate and bind vertex buffer object, buffer data
    glCreateBuffers(1, &VBO_grid);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_grid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_grid), vertices_grid, GL_STATIC_DRAW);

    // vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
}

void Renderer::init_properties() {
    // initialize object positions
    glm::vec3 position_cube{ glm::vec3(0.0f, 0.5f, 0.0f) };
    glm::vec3 position_axis{ glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 position_grid{ glm::vec3(0.0f, 0.0f, 0.0f) };

    // initialize axis colors
    axis_colors[0] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);      // x-axis
    axis_colors[1] = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);      // y-axis
    axis_colors[2] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);      // z-axis

    // initialize axis rotations
    axis_rotations[0] = std::make_pair(glm::vec3(1.0f, 0.0f, 0.0f), 360.0f);    // x-axis
    axis_rotations[1] = std::make_pair(glm::vec3(0.0f, 0.0f, 1.0f),  90.0f);    // y-axis
    axis_rotations[2] = std::make_pair(glm::vec3(0.0f, 1.0f, 0.0f), -90.0f);    // z-axis
}