#ifndef SHADER_H
#define SHADER_H

// project headers
#include "constants.h"

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
    Shader(const std::string& pathVertex,
        const std::string& pathFragment,
        const std::string& pathGeometry = std::string());
    Shader(const Shader& shader)
        : m_programID{ shader.m_programID } {}
    Shader(Shader&& shader)
        : m_programID{ std::move(shader.m_programID) } {}

    // getters
    GLuint getProgramID() const;

    // setters
    void setUniformFloat(const std::string& uniform,
        GLfloat value) const;
    void setUniformUInt(const std::string& uniform,
        GLuint value) const;
    void setUniformBool(const std::string& uniform,
        bool value) const;
    void setUniformMat4(const std::string& uniform,
        const glm::mat4& value) const;
    void setUniformVec2(const std::string& uniform,
        const glm::vec2& value) const;
    void setUniformVec3(const std::string& uniform,
        const glm::vec3& value) const;
    void setUniformVec4(const std::string& uniform,
        const glm::vec4& value) const;

    // utilities
    static void useProgram(GLuint programID);
    static void validateProgram(GLuint programID);
    void free() const;
    void setColorAttributes(GLuint VAO,
        GLuint VBO,
        GLuint EBO = NULL);
    void setDepthAttributes(GLuint VAO,
        GLuint VBO,
        GLuint EBO = NULL);
    void use() const;
    void validate() const;

private:
    void compileShader(const std::string& shaderType,
        GLuint shaderID) const;
    void linkProgram(GLuint shaderVertex,
        GLuint shaderFragment,
        GLuint shaderGeometry = NULL);

    GLuint m_programID;
};

#endif // !SHADER_H