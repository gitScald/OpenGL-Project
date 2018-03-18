#ifndef RENDERED_ENTITY_H
#define RENDERED_ENTITY_H

// project headers
#include "constants.h"
#include "enums.h"
#include "material.h"
#include "shader.h"

// GLEW
#include <gl/glew.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ standard library headers
#include <vector>

class RenderedEntity {
public:
    RenderedEntity() = delete;
    RenderedEntity(Shader* shader,
        const glm::vec3& position,
        GLfloat vertices[],
        GLuint verticesSize,
        GLuint indices[] = nullptr,
        GLuint indicesSize = 0)
        : m_position { position },
        m_positionOriginal{ position } {
        initialize(vertices,
            verticesSize,
            indices,
            indicesSize);
    }
    RenderedEntity(const RenderedEntity& entity)
        : m_rotationMatrix{ entity.m_rotationMatrix },
        m_rotationMatrixOriginal{ entity.m_rotationMatrixOriginal },
        m_scalingMatrix{ entity.m_scalingMatrix },
        m_translationMatrix{ entity.m_translationMatrix },
        m_modelMatrix{ entity.m_modelMatrix },
        m_position{ entity.m_position },
        m_positionOriginal{ entity.m_positionOriginal },
        m_scalingOriginal{ entity.m_scalingOriginal },
        m_scalingRelative{ entity.m_scalingRelative },
        m_color{ entity.m_color },
        m_VAO{ entity.m_VAO },
        m_VBO{ entity.m_VBO },
        m_EBO{ entity.m_EBO },
        m_vertexCount{ entity.m_vertexCount },
        m_rotationSet{ entity.m_rotationSet },
        m_scaleSet{ entity.m_scaleSet },
        m_translationSet{ entity.m_translationSet } {}
    RenderedEntity(RenderedEntity&& entity)
        : m_rotationMatrix{ std::move(entity.m_rotationMatrix) },
        m_rotationMatrixOriginal{ std::move(entity.m_rotationMatrixOriginal) },
        m_scalingMatrix{ std::move(entity.m_scalingMatrix) },
        m_translationMatrix{ std::move(entity.m_translationMatrix) },
        m_modelMatrix{ std::move(entity.m_modelMatrix) },
        m_position{ std::move(entity.m_position) },
        m_positionOriginal{ std::move(entity.m_positionOriginal) },
        m_scalingOriginal{ std::move(entity.m_scalingOriginal) },
        m_scalingRelative{ std::move(entity.m_scalingRelative) },
        m_color{ std::move(entity.m_color) },
        m_VAO{ std::move(entity.m_VAO) },
        m_VBO{ std::move(entity.m_VBO) },
        m_EBO{ std::move(entity.m_EBO) },
        m_vertexCount{ std::move(entity.m_vertexCount) },
        m_rotationSet{ std::move(entity.m_rotationSet) },
        m_scaleSet{ std::move(entity.m_scaleSet) },
        m_translationSet{ std::move(entity.m_translationSet) } {}
    RenderedEntity& operator=(RenderedEntity& entity) = delete;

    // getters
    glm::mat4 getModelMatrix(const glm::mat4& globalModelMatrix);
    glm::mat4 getModelMatrixExclScaling(const glm::mat4& jointModelMatrix);
    const glm::mat4& getScalingMatrix() const;
    const glm::vec3& getScalingRelative() const;

    // setters
    void setColor(const glm::vec4& value);
    void setPosition(const glm::vec3& value);

    // transformations
    void move(Transform::Displacement direction);
    void reset();
    void rotate(GLfloat angle, const glm::vec3& axis);
    void scale(const glm::vec3& value);
    void translate(const glm::vec3& value);

    // utilities
    void render(Rendering::Primitive primitive) const;
    void setColorShaderAttributes(Shader* shader) const;
    void setDepthShaderAttributes(Shader* shader) const;

private:
    void initialize(GLfloat vertices[],
        GLuint verticesSize,
        GLuint indices[] = nullptr,
        GLuint indicesSize = 0);

    glm::mat4 m_rotationMatrix;
    glm::mat4 m_rotationMatrixOriginal;
    glm::mat4 m_scalingMatrix;
    glm::mat4 m_translationMatrix;
    glm::mat4 m_modelMatrix;
    glm::vec3 m_position;
    glm::vec3 m_positionOriginal;
    glm::vec3 m_scalingOriginal;
    glm::vec3 m_scalingRelative{ glm::vec3(1.0f, 1.0f, 1.0f) };
    glm::vec4 m_color;
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO{ NULL };
    GLuint m_vertexCount;
    bool m_rotationSet{ false };
    bool m_scaleSet{ false };
    bool m_translationSet{ false };
};

#endif // !RENDERED_ENTITY_H