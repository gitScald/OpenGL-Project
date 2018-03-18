#ifndef JOINT_H
#define JOINT_H

// project headers
#include "constants.h"
#include "rendered_entity.h"

class Joint {
public:
    Joint() = delete;
    Joint(RenderedEntity* parent,
        RenderedEntity* child,
        const glm::vec3& position,
        GLfloat rotationMax = 360.0f,
        GLfloat rotationMin = 360.0f)
        : m_parent{ parent },
        m_child{ child },
        m_position{ position },
        m_rotationMax{ rotationMax },
        m_rotationMin{ rotationMin } {}
    Joint(const Joint& joint)
        : m_parent{ joint.m_parent },
        m_child{ joint.m_child },
        m_position{ joint.m_position },
        m_rotationAngle{ joint.m_rotationAngle },
        m_rotationMax{ joint.m_rotationMax },
        m_rotationMin{ joint.m_rotationMin },
        m_rotationMatrix{ joint.m_rotationMatrix } {}
    Joint(Joint&& joint)
        : m_parent{ std::move(joint.m_parent) },
        m_child{ std::move(joint.m_child) },
        m_position{ std::move(joint.m_position) },
        m_rotationAngle{ std::move(joint.m_rotationAngle) },
        m_rotationMax{ std::move(joint.m_rotationMax) },
        m_rotationMin{ std::move(joint.m_rotationMin) },
        m_rotationMatrix{ std::move(joint.m_rotationMatrix) } {}
    Joint* operator=(Joint& joint) = delete;

    // getters
    glm::mat4 getModelMatrix() const;

    // transformations
    void reset();
    void rotate(GLfloat angle, const glm::vec3& axis);

    // utilities
    bool contains(RenderedEntity* entity) const;

private:
    void clampRotation();

    glm::mat4 m_rotationMatrix;
    glm::vec3 m_position;
    RenderedEntity* m_parent;
    RenderedEntity* m_child;
    GLfloat m_rotationAngle{ 0.0f };
    GLfloat m_rotationMax;
    GLfloat m_rotationMin;
};

#endif // !JOINT_H