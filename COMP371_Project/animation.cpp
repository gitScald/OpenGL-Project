#include "animation.h"

void Animation::addStep(AnimationStep* step) {
    // add animation step to sequence
    m_animationSequence.push_back(step);
    if (m_keyframeLast < step->m_keyframe)
        m_keyframeLast = step->m_keyframe;
}

void Animation::setSpeed(GLfloat speed) {
    // set animation speed
    m_speed = speed;
}

void Animation::play(Model* model,
    GLfloat deltaTime) {
    // play current keyframe
    bool keyframeUpdated{ false };
    for (AnimationSequence::const_iterator it{ m_animationSequence.begin() };
        it != m_animationSequence.end();
        ++it) {
        if ((*it)->m_keyframe == m_keyframe) {
            model->rotateJoint((*it)->m_joint,
                (*it)->m_rotationAngle * deltaTime,
                (*it)->m_rotationAxis);

            // update current frame
            if (!keyframeUpdated
                && model->getJointRotation((*it)->m_joint)
                    >= (*it)->m_rotationAngle) {
                m_keyframe = (m_keyframe + 1) % (m_keyframeLast + 1);
                keyframeUpdated = true;
            }
        }
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