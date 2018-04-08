#ifndef PATH_H
#define PATH_H

// project headers
#include "model.h"
#include "path_step.h"

// C++ standard library headers
#include <vector>

class Path {
public:
    // sequence of path steps
    typedef std::vector<PathStep*> PathSequence;

    Path() {}
    Path(const Path& path)
        : m_pathSequence{ path.m_pathSequence },
        m_step{ path.m_step },
        m_stepLast{ path.m_stepLast } {}
    Path(Path&& path)
        : m_pathSequence{ std::move(path.m_pathSequence) },
        m_step{ std::move(path.m_step) },
        m_stepLast{ std::move(path.m_stepLast) } {}
    Path& operator=(Path& path) = delete;

    // setters
    void addStep(PathStep* step);
    void setSpeed(GLfloat speed);

    // utilities
    void traverse(Model* model,
        GLfloat deltaTime);

private:
    PathSequence m_pathSequence;
    GLuint m_step{ 0 };
    GLuint m_stepLast{ 1 };
    GLfloat m_timeTotal{ 0.0f };
    GLfloat m_timeTraveled{ 0.0f };
    GLfloat m_speed{ PATHING_SPEED };
};

#endif // !PATH_H
