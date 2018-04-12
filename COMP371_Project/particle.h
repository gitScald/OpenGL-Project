#ifndef PARTICLE_H
#define PARTICLE_H

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>

struct Particle {
public:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    GLfloat m_life;
    GLfloat m_distanceToCamera;

    bool operator<(const Particle& particle) const {
        return this->m_distanceToCamera > particle.m_distanceToCamera
            || this->m_position.y < particle.m_position.y;
    }
};

#endif // !PARTICLE_H
