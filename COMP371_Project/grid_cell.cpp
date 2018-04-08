#include "grid_cell.h"

GridCell::Collision GridCell::checkForCollisions() {
    // return empty collision vector if there are less than two entities
    Collision collision;
    if (m_models.size() < 2)
        return collision;

    // keep going
    return collision;
}

void GridCell::updateModelsInCell(const std::vector<Model*>& models) {
    // clear models list
    m_models.clear();

    // update models list
    for (std::vector<Model*>::const_iterator it{ models.begin() };
        it != models.end();
        ++it) {
        glm::vec3 position = (*it)->getPosition();
        if (position.x >= m_x && position.x < (m_x + m_size)
            && position.z >= m_z && position.z < (m_z + m_size))
            m_models.push_back((*it));
    }
}