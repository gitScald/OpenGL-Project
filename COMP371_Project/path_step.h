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
        GLfloat direction,
        GLfloat time)
        : m_step{ step },
        m_direction{ direction },
        m_time{ time } {}
    PathStep(const PathStep& step)
        : m_step{ step.m_step },
        m_direction{ step.m_direction },
        m_time{ step.m_time } {}
    PathStep(PathStep&& step)
        : m_step{ std::move(step.m_step) },
        m_direction{ std::move(step.m_direction) },
        m_time{ std::move(step.m_time) } {}
    PathStep& operator=(PathStep& step) = delete;

    GLuint m_step;
    GLfloat m_direction;
    GLfloat m_time;
};

#endif // !PATH_STEP_H
