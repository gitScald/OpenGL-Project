#include "shader.h"

// initially bound buffer objects
GLuint Shader::s_VAO = NULL;
GLuint Shader::s_VBO = NULL;
GLuint Shader::s_EBO = NULL;

// initially bound shader program
GLuint Shader::s_program = NULL;

// initially bound texture
GLuint Shader::s_2Dtexture = NULL;

Shader::Shader(const std::string& pathVertex,
    const std::string& pathFragment,
    const std::string& pathGeometry) {
    // create shader program from specified shader files
    std::ifstream ifsVertex, ifsFragment, ifsGeometry;
    std::stringstream ssVertex, ssFragment, ssGeometry;
    std::string codeVertex, codeFragment, codeGeometry;

    // enable ifstream exceptions to be thrown
    ifsVertex.exceptions(std::ifstream::failbit
        | std::ifstream::badbit);
    ifsFragment.exceptions(std::ifstream::failbit
        | std::ifstream::badbit);
    ifsGeometry.exceptions(std::ifstream::failbit
        | std::ifstream::badbit);

    // read shader code
    try {
        ifsVertex.open(pathVertex);
        ssVertex << ifsVertex.rdbuf();
        codeVertex = ssVertex.str();
        ifsVertex.close();
    }
    catch (std::ifstream::failure e) {
        std::cout << ">>> Failed to establish input stream "
            << "with vertex shader file: \"" << pathVertex << "\""
            << std::endl << e.what() << std::endl;
    }
    const GLchar* srcVertex = codeVertex.c_str();

    try {
        ifsFragment.open(pathFragment);
        ssFragment << ifsFragment.rdbuf();
        codeFragment = ssFragment.str();
        ifsFragment.close();
    }
    catch (std::ifstream::failure e) {
        std::cout << ">>> Failed to establish input stream "
            << "with fragment shader file: \"" << pathFragment << "\""
            << std::endl << e.what() << std::endl;
    }
    const GLchar* srcFragment = codeFragment.c_str();

    if (!pathGeometry.empty()) {
        try {
            ifsGeometry.open(pathGeometry);
            ssGeometry << ifsGeometry.rdbuf();
            codeGeometry = ssGeometry.str();
            ifsGeometry.close();
        }
        catch (std::ifstream::failure e) {
            std::cout << ">>> Failed to establish input stream "
                << "with geometry shader file: \"" << pathGeometry << "\""
                << std::endl << e.what() << std::endl;
        }
    }
    const GLchar* srcGeometry = codeGeometry.c_str();

    // id's for the shaders
    GLuint shaderVertex, shaderFragment, shaderGeometry;

    // create and compile vertex shader
    shaderVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVertex, 1, &srcVertex, NULL);
    std::cout << "Compiling vertex shader from source: \""
        << pathVertex << "\"..." << std::endl;
    compileShader("GL_VERTEX_SHADER", shaderVertex);

    // create and compile fragment shader
    shaderFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderFragment, 1, &srcFragment, NULL);
    std::cout << "Compiling fragment shader from source: \""
        << pathFragment << "\"..." << std::endl;
    compileShader("GL_FRAGMENT_SHADER", shaderFragment);

    // create and compile geometry shader
    if (!pathGeometry.empty()) {
        shaderGeometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(shaderGeometry, 1, &srcGeometry, NULL);
        std::cout << "Compiling geometry shader from source: \""
            << pathGeometry << "\"..." << std::endl;
        compileShader("GL_GEOMETRY_SHADER", shaderGeometry);

        // create and link shader program
        linkProgram(shaderVertex, shaderFragment, shaderGeometry);
    }
    else
        // create and link shader program
        linkProgram(shaderVertex, shaderFragment);

    // free shaders
    glDeleteShader(shaderVertex);
    glDeleteShader(shaderFragment);
    if (!pathGeometry.empty())
        glDeleteShader(shaderGeometry);
}

GLuint Shader::getProgramID() const {
    // return shader program id
    return m_programID;
}

void Shader::setUniformFloat(const std::string& uniform,
    GLfloat value) const {
    // set a float uniform
    glUniform1f(glGetUniformLocation(m_programID,
        uniform.c_str()),
        value);
}

void Shader::setUniformUInt(const std::string& uniform,
    GLuint value) const {
    // set an unsigned int uniform
    glUniform1i(glGetUniformLocation(m_programID,
        uniform.c_str()),
        value);
}

void Shader::setUniformBool(const std::string& uniform,
    bool value) const {
    // set a bool uniform
    setUniformUInt(uniform, static_cast<GLuint>(value));
}

void Shader::setUniformMat4(const std::string& uniform,
    const glm::mat4& value) const {
    // set a mat4 uniform
    glUniformMatrix4fv(glGetUniformLocation(m_programID,
        uniform.c_str()),
        1,
        GL_FALSE,
        &value[0][0]);
}

void Shader::setUniformVec2(const std::string& uniform,
    const glm::vec2& value) const {
    // set a vec2 uniform
    glUniform2fv(glGetUniformLocation(m_programID,
        uniform.c_str()),
        1,
        &value[0]);
}

void Shader::setUniformVec3(const std::string& uniform,
    const glm::vec3& value) const {
    // set a vec3 uniform
    glUniform3fv(glGetUniformLocation(m_programID,
        uniform.c_str()),
        1,
        &value[0]);
}

void Shader::setUniformVec4(const std::string& uniform,
    const glm::vec4& value) const {
    // set a vec4 uniform
    glUniform4fv(glGetUniformLocation(m_programID,
        uniform.c_str()),
        1,
        &value[0]);
}

void Shader::bindVAO(GLuint VAO) {
    // bind VAO if needed
    if (VAO != s_VAO) {
        glBindVertexArray(VAO);
        s_VAO = VAO;
    }
}

void Shader::bindVBO(GLuint VBO) {
    // bind VBO if needed
    if (VBO != s_VBO) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        s_VBO = VBO;
    }
}

void Shader::bindEBO(GLuint EBO) {
    // bind EBO if needed
    if (EBO != s_EBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        s_EBO = EBO;
    }
}

void Shader::bind2DTexture(GLuint texture) {
    // bind texture if needed
    if (texture != s_2Dtexture) {
        glBindTexture(GL_TEXTURE_2D, texture);
        s_2Dtexture = texture;
    }
}

void Shader::useProgram(GLuint programID) {
    // use given shader program if needed
    if (programID != s_program)
        glUseProgram(programID);
}

void Shader::validateProgram(GLuint programID) {
    // perform validation on given shader program
    glValidateProgram(programID);

    GLint status;
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);
    if (!status) {
        GLchar log[512];
        glGetProgramInfoLog(programID, 512, NULL, log);
        std::cerr << ">>> Shader validation failed:"
            << std::endl << log << std::endl;
    }
    else
        std::cout << "Shader validation passed." << std::endl;
    std::cout << std::endl;
}

void Shader::free() const {
    // free shader program
    glDeleteProgram(m_programID);
}

void Shader::setColorAttributes(GLuint VAO,
    GLuint VBO,
    GLuint EBO) {
    // bind buffers
    bindVAO(VAO);
    bindVBO(VBO);
    if (EBO != NULL)
        bindEBO(EBO);

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_programID,
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);
    GLuint normalLocation = glGetAttribLocation(m_programID,
        ATTRIBUTE_NORMAL.c_str());
    glVertexAttribPointer(normalLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(normalLocation);
    GLuint textureLocation = glGetAttribLocation(m_programID,
        ATTRIBUTE_TEXTURE.c_str());
    glVertexAttribPointer(textureLocation,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(textureLocation);
}

void Shader::setDepthAttributes(GLuint VAO,
    GLuint VBO,
    GLuint EBO) {
    // bind buffers
    bindVAO(VAO);
    bindVBO(VBO);
    if (EBO != NULL)
        bindEBO(EBO);

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_programID,
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);
}

void Shader::compileShader(const std::string& shaderType,
    GLuint shaderID) const {
    // attempt to compile shader
    glCompileShader(shaderID);

    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLchar log[512];
        glGetShaderInfoLog(shaderID, 512, NULL, log);
        std::cerr << ">>> Failed to compile shader: "
            << shaderType << std::endl << log;
    }
    else
        std::cout << "Shader compilation successful: "
        << shaderType;
    std::cout << std::endl;
}

void Shader::linkProgram(GLuint shaderVertex,
    GLuint shaderFragment,
    GLuint shaderGeometry) {
    // attempt to link program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, shaderVertex);
    glAttachShader(m_programID, shaderFragment);
    if (shaderGeometry)
        glAttachShader(m_programID, shaderGeometry);
    glLinkProgram(m_programID);

    GLint status;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &status);
    if (!status) {
        GLchar log[512];
        glGetProgramInfoLog(m_programID, 512, NULL, log);
        std::cerr << ">>> Failed to link shader program: "
            << std::endl << log << std::endl;
    }
    else
        std::cout << "Shader program linking successful." << std::endl;
    std::cout << std::endl;
}