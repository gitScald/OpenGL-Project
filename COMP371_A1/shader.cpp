#include "shader.h"

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
    }
    catch (std::ifstream::failure e) {
        std::cout << "Failed to establish input stream with vertex shader file: \"" << path_vertex << "\"" << std::endl << e.what() << std::endl;
    }
    const GLchar* src_vertex = code_vertex.c_str();

    try {
        ifs_fragment.open(path_fragment);
        ss_fragment << ifs_fragment.rdbuf();
        code_fragment = ss_fragment.str();
        ifs_fragment.close();
    }
    catch (std::ifstream::failure e) {
        std::cout << "Failed to establish input stream with fragment shader file: \"" << path_fragment << "\"" <<std::endl << e.what() << std::endl;
    }
    const GLchar* src_fragment = code_fragment.c_str();

    // id's for the two shaders
    GLuint shdr_vertex, shdr_fragment;

    // create and compile vertex shader
    shdr_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shdr_vertex, 1, &src_vertex, NULL);
    std::cout << "Compiling vertex shader from source: \"" << path_vertex << "\"..." << std::endl;
    compile_shader("GL_VERTEX_SHADER", shdr_vertex);

    // create and compile fragment shader
    shdr_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shdr_fragment, 1, &src_fragment, NULL);
    std::cout << "Compiling fragment shader from source: \"" << path_fragment << "\"..." << std::endl;
    compile_shader("GL_FRAGMENT_SHADER", shdr_fragment);

    // create and link shader program
    link_program(shdr_vertex, shdr_fragment);

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

void Shader::set_vec4(const std::string& uniform, const glm::vec4& value) const {
    // sets a vec4 uniform
    glUniform4fv(glGetUniformLocation(id, uniform.c_str()), 1, &value[0]);
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

void Shader::link_program(GLuint shdr_vertex, GLuint shdr_fragment) {
    // attempt to link program and report any issues
    id = glCreateProgram();
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
        std::cout << "Shader program linking successful." << std::endl;
}