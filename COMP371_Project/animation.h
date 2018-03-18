#ifndef ANIMATION_H
#define ANIMATION_H

// project headers
#include "animation_step.h"
#include "model.h"

class Animation {
public:
    // sequence of animation steps
    typedef std::vector<AnimationStep> AnimationSequence;

    Animation() = delete;
    Animation(Model* model)
        : m_model{ model } {}
    Animation(const Animation& animation)
        : m_model{ animation.m_model },
        m_animationSequence{ animation.m_animationSequence },
        m_keyframe{ animation.m_keyframe },
        m_keyframeLast{ animation.m_keyframeLast },
        m_frame{ animation.m_frame },
        m_speed{ animation.m_speed } {}
    Animation(Animation&& animation)
        : m_model{ std::move(animation.m_model) },
        m_animationSequence{ std::move(animation.m_animationSequence) },
        m_keyframe{ std::move(animation.m_keyframe) },
        m_keyframeLast{ std::move(animation.m_keyframeLast) },
        m_frame{ std::move(animation.m_frame) },
        m_speed{ std::move(animation.m_speed) } {}
    Animation& operator=(Animation& animation) = delete;

    // setters
    void addStep(AnimationStep step);
    void setSpeed(GLfloat speed);

    // utilities
    void play(GLfloat deltaTime);

private:
    GLuint rotationDirection(GLfloat angle,
        const glm::vec3& axis) const;

    AnimationSequence m_animationSequence;
    Model* m_model;
    GLuint m_keyframe{ 0 };
    GLuint m_keyframeLast{ 1 };
    GLfloat m_frame{ 0.0f };
    GLfloat m_speed{ ANIMATION_SPEED };
};

#endif // !ANIMATION_H