#ifndef RENDERER_H
#define RENDERER_H

// project headers
#include "animation.h"
#include "camera.h"
#include "constants.h"
#include "enums.h"
#include "light_source.h"
#include "material.h"
#include "model.h"
#include "rendered_entity.h"
#include "shader.h"
#include "shadow_map.h"
#include "texture.h"

// GLEW
#include <gl/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ standard library headers
#include <stdlib.h>
#include <vector>

// singleton
class Renderer {
public:
    Renderer(const Renderer& renderer) = delete;
    Renderer(Renderer&& renderer) = delete;
    ~Renderer();
    Renderer& operator=(Renderer const& renderer) = delete;
    static Renderer& get();

    // getters
    Rendering::Primitive getPrimitive() const;
    GLfloat getAnimationSpeed() const;
    GLfloat getAnimationSpeedCurrent() const;

    // setters
    void setPrimitive(Rendering::Primitive primitive);
    void setAnimationSpeedCurrent(GLfloat value);

    // transformations
    void moveModel(Transform::Displacement direction);
    void resetModel();
    void rotateModel(Transform::Displacement direction);
    void rotateModelJoint(GLuint id,
        Transform::Displacement direction);
    void scaleModel(Transform::Scale value);

    // utilities
    void render(GLfloat deltaTime);
    void toggleAnimations();
    void toggleDebugging();
    void toggleFrame();
    void toggleLights();
    void toggleShadows();
    void toggleTextures();

private:
    Renderer()
        : m_shaderEntity{ PATH_VERTEX_ENTITY,
            PATH_FRAGMENT_ENTITY },
        m_shaderFrame{ PATH_VERTEX_FRAME,
            PATH_FRAGMENT_FRAME },
        m_shaderShadow{ PATH_VERTEX_SHADOW,
            PATH_FRAGMENT_SHADOW,
            PATH_GEOMETRY_SHADOW } {
        initialize();
    }

    // initialization
    void initialize();
    void initializeAnimation();
    void initializeFrame();
    void initializeGround();
    void initializeLight();
    void initializeModel();

    // rendering passes
    void renderFirstPass(GLfloat deltaTime);
    void renderSecondPass(GLfloat deltaTime);

    // rendered elements
    void renderFrame();
    void renderGround(Shader* shader);
    void renderLight();
    void renderModels(Shader* shader, GLfloat deltaTime);

    // rendering utilities
    const glm::mat4& getWorldOrientation() const;
    glm::vec3 getWorldAxis(const glm::vec3& axis) const;

    // transformations
    void clampModelPosition();
    void clampModelScale();
    
    static Renderer& s_instance;
    Rendering::Primitive m_primitive{ Rendering::TRIANGLES };
    std::vector<Animation> m_animations;
    std::vector<Joint> m_joints;
    std::vector<LightSource> m_lights;
    std::vector<Material> m_materials;
    std::vector<Model> m_models;
    std::vector<RenderedEntity> m_entities;
    glm::mat4 m_modelMatrix;
    glm::vec3 m_modelPosition{ MODEL_POSITION_RELATIVE_TORSO };
    glm::vec3 m_modelScale{ glm::vec3(1.0f, 1.0f, 1.0f) };
    Shader m_shaderEntity;
    Shader m_shaderFrame;
    Shader m_shaderShadow;
    ShadowMap m_shadowMap;
    GLuint m_axesVAO;
    GLuint m_axesVBO;
    GLuint m_gridVAO;
    GLuint m_gridVBO;
    GLuint m_lightVAO;
    GLuint m_lightVBO;
    GLfloat m_animationSpeed{ ANIMATION_SPEED };
    GLfloat m_animationSpeedCurrent{ ANIMATION_SPEED };
    bool m_animationsEnabled{ false };
    bool m_debuggingEnabled{ false };
    bool m_frameEnabled{ true };
    bool m_lightsEnabled{ true };
    bool m_shadowsEnabled{ true };
    bool m_texturesEnabled{ true };
};

#endif // !RENDERER_H