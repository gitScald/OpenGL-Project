#include "animation.h"

void Animation::addStep(AnimationStep step) {
    // add animation step to sequence
    m_animationSequence.emplace_back(std::move(step));
    if (m_keyframeLast < step.m_keyframe)
        m_keyframeLast = step.m_keyframe;
}

void Animation::setSpeed(GLfloat speed) {
    // set animation speed
    m_speed = speed;
}

void Animation::play(GLfloat deltaTime) {
    // update current frame
    m_frame += m_speed * deltaTime;
    m_keyframe = static_cast<GLuint>(m_frame) % (m_keyframeLast + 1);

    // play current keyframe
    for (AnimationSequence::const_iterator it{ m_animationSequence.begin() };
        it != m_animationSequence.end();
        ++it) {
        if (it->m_keyframe == m_keyframe)
            m_model->rotateJoint(it->m_joint,
                it->m_rotationAngle,
                it->m_rotationAxis);
    }
}

GLuint Animation::rotationDirection(GLfloat angle,
    const glm::vec3& axis) const {
    // return rotation direction (CW or CCW)
    if (axis.x < 0 || axis.y < 0 || axis.z < 0) {
        if (angle > 0)
            return -1;
        return 1;
    }

    if (angle > 0)
        return 1;
    return -1;
}