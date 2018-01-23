#ifndef GLOBALS_H
#define GLOBALS_H

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>

// C++ standard library headers
#include <string>

// window creation constants
const GLuint GL_VERSION_MAJOR = 3;
const GLuint GL_VERSION_MINOR = 3;
const GLuint SCREEN_WIDTH = 800;
const GLuint SCREEN_HEIGHT = 800;
const GLfloat ASPECT_RATIO = SCREEN_WIDTH / SCREEN_HEIGHT;
const std::string WINDOW_TITLE = "COMP 371 Assignment 1";

// camera-related constants
const GLfloat INIT_YAW{ -180.0f };
const GLfloat INIT_PITCH{ -89.0f };
const GLfloat INIT_SPEED{ 2.5f };
const GLfloat INIT_SENSITIVITY{ 0.1f };
const GLfloat Z_NEAR = 0.1f;
const GLfloat Z_FAR = 1000.0f;
const GLfloat MAX_PITCH = 89.0f;
const GLfloat MAX_FOV = 45.0f;
const GLfloat MIN_FOV = 1.0f;

// shader file paths
const std::string PATH_VERTEX_CUBE{ "shaders/cube/vertex.shdr" };
const std::string PATH_FRAGMENT_CUBE{ "shaders/cube/fragment.shdr" };
const std::string PATH_VERTEX_AXIS{ "shaders/axis/vertex.shdr" };
const std::string PATH_FRAGMENT_AXIS{ "shaders/axis/fragment.shdr" };
const std::string PATH_VERTEX_GRID{ "shaders/grid/vertex.shdr" };
const std::string PATH_FRAGMENT_GRID{ "shaders/grid/fragment.shdr" };


#endif // !GLOBALS_H