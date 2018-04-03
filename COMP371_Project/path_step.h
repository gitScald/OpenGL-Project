#ifndef PATH_STEP_H
#define PATH_STEP_H

// GLEW
#include <gl/glew.h>

// C++ standard library headers
#include <utility>

struct PathStep {
public:
    PathStep() = delete;
    PathStep(GLuint step,
        GLfloat direction)
        : m_step{ step },
        m_direction{ direction } {}
    PathStep(const PathStep& step)
        : m_step{ step.m_step },
        m_direction{ std::move(step.m_direction) } {}
    PathStep(PathStep&& step)
        : m_step{ std::move(step.m_step) },
        m_direction{ std::move(step.m_direction) } {}
    PathStep& operator=(PathStep& step) = delete;

    GLuint m_step;
    GLfloat m_direction;
};

#endif // !PATH_STEP_H
