#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

// project headers
#include "constants.h"
#include "enums.h"
#include "shader.h"

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class LightSource {
public:
    LightSource() = delete;
    LightSource(const glm::vec3& position)
        : m_position{ position },
        m_positionOriginal{ position } {}
    LightSource(const LightSource& light)
        : m_position{ light.m_position },
        m_positionOriginal{ light.m_positionOriginal },
        m_ambient{ light.m_ambient },
        m_diffuse{ light.m_diffuse },
        m_specular{ light.m_specular },
        m_KC{ light.m_KC },
        m_KL{ light.m_KL },
        m_KQ{ light.m_KQ },
        m_planeNear{ light.m_planeNear },
        m_planeFar{ light.m_planeFar } {}
    LightSource(LightSource&& light)
        : m_position{ std::move(light.m_position) },
        m_positionOriginal{ std::move(light.m_positionOriginal) },
        m_ambient{ std::move(light.m_ambient) },
        m_diffuse{ std::move(light.m_diffuse) },
        m_specular{ std::move(light.m_specular) },
        m_KC{ std::move(light.m_KC) },
        m_KL{ std::move(light.m_KL) },
        m_KQ{ std::move(light.m_KQ) },
        m_planeNear{ std::move(light.m_planeNear) },
        m_planeFar{ std::move(light.m_planeFar) } {}
    LightSource& operator=(LightSource& light) = delete;

    // getters
    const glm::vec3& getPosition() const;
    const glm::vec3& getAmbient() const;
    const glm::vec3& getDiffuse() const;
    const glm::vec3& getSpecular() const;
    glm::vec3 getWorldPosition(const glm::mat4& globalModelMatrix) const;
    GLfloat getKC() const;
    GLfloat getKL() const;
    GLfloat getKQ() const;
    GLfloat getPlaneNear() const;
    GLfloat getPlaneFar() const;

    // setters
    void setPosition(const glm::vec3& value);

    // movement
    void move(Transform::Displacement direction);
    void reset();
    void translate(const glm::vec3& value);

private:
    glm::vec3 m_position;
    glm::vec3 m_positionOriginal;
    glm::vec3 m_ambient{ LIGHT_AMBIENT };
    glm::vec3 m_diffuse{ LIGHT_DIFFUSE };
    glm::vec3 m_specular{ LIGHT_SPECULAR };
    GLfloat m_KC{ LIGHT_KC };
    GLfloat m_KL{ LIGHT_KL };
    GLfloat m_KQ{ LIGHT_KQ };
    GLfloat m_planeNear{ LIGHT_PLANE_NEAR };
    GLfloat m_planeFar{ LIGHT_PLANE_FAR };
};

#endif // !LIGHT_SOURCE_H