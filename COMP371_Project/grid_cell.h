#ifndef GRID_CELL_H
#define GRID_CELL_H

// project headers
#include "model.h"

// GLEW
#include <gl/glew.h>

// C++ standard library headers
#include <list>
#include <vector>

class GridCell {
public:
    // vector of colliding models
    typedef std::vector<std::pair<Model*, Model*>> Collision;

    GridCell() = delete;
    GridCell(GLint x, GLint z, GLuint size)
        : m_x{ x },
        m_z{ z },
        m_size{ size } {}
    GridCell(const GridCell& cell) = delete;
    GridCell(GridCell&& cell) = delete;
    GridCell& operator=(GridCell& cell) = delete;

    // utilities
    Collision checkForCollisions();
    void updateModelsInCell(const std::vector<Model*>& models);

private:
    std::list<Model*> m_models;
    GLint m_x;
    GLint m_z;
    GLuint m_size;
};

#endif // !GRID_CELL_H
