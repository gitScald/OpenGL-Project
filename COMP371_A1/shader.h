#ifndef SHADER_H
#define SHADER_H

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>

// C++ standard library headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:
    Shader() = delete;
    Shader(const std::string& path_vertex, const std::string& path_fragment);
    Shader(Shader& shdr) = delete;
    ~Shader();

    void set_mat4(const std::string& uniform, const glm::mat4& value) const;
    void set_vec4(const std::string& uniform, const glm::vec4& value) const;
    void use() const;

private:
    void compile_shader(const std::string& shdr_type, GLuint shdr_id) const;
    void link_program(GLuint shdr_vertex, GLuint shdr_fragment);

    GLuint id;
};

#endif // !SHADER_H