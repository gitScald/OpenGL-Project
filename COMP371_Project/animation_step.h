#ifndef ANIMATION_STEP_H
#define ANIMATION_STEP_H

// project headers
#include "joint.h"

struct AnimationStep {
public:
    AnimationStep() = delete;
    AnimationStep(GLuint joint,
        const glm::vec3& rotationAxis,
        GLuint keyframe,
        GLfloat rotationAngle)
        : m_joint{ joint },
        m_rotationAxis{ rotationAxis },
        m_keyframe{ keyframe },
        m_rotationAngle{ rotationAngle } {}
    AnimationStep(const AnimationStep& step)
        : m_joint{ step.m_joint },
        m_rotationAxis{ step.m_rotationAxis },
        m_keyframe{ step.m_keyframe },
        m_rotationAngle{ step.m_rotationAngle } {}
    AnimationStep(AnimationStep&& step)
        : m_joint{ std::move(step.m_joint) },
        m_rotationAxis{ std::move(step.m_rotationAxis) },
        m_keyframe{ std::move(step.m_keyframe) },
        m_rotationAngle{ std::move(step.m_rotationAngle) } {}
    AnimationStep& operator=(AnimationStep& step) = delete;

    GLuint m_joint;
    glm::vec3 m_rotationAxis;
    GLuint m_keyframe;
    GLfloat m_rotationAngle;
};

#endif // !ANIMATION_STEP_H
