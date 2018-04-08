#ifndef RENDERER_H
#define RENDERER_H

// project headers
#include "animation.h"
#include "camera.h"
#include "constants.h"
#include "enums.h"
#include "light_source.h"
//#include "loader.h"
#include "material.h"
#include "model.h"
#include "path.h"
#include "rendered_entity.h"
#include "shader.h"
#include "shadow_map.h"
#include "skybox.h"
#include "texture.h"

// GLEW
#include <gl/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ standard library headers
#include <stdlib.h>
#include <time.h>
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
    void moveModel(GLuint model,
        Transform::Displacement direction);
    void resetModel(GLuint model);
    void rotateModel(GLuint model,
        Transform::Displacement direction,
        GLfloat angle = TRANSFORMATION_INCREMENT_ROTATION);
    void rotateModelJoint(GLuint model,
        GLuint id,
        Transform::Displacement direction);
    void scaleModel(GLuint model,
        Transform::Scale value);

    // utilities
    void render(GLfloat deltaTime);
    void toggleAnimations();
    void toggleDebugging();
    void toggleFrame();
    void toggleLights();
    void togglePathing();
    void toggleShadows();
    void toggleTextures();

private:
    Renderer()
        : m_shaderEntity{ new Shader(PATH_VERTEX_ENTITY,
            PATH_FRAGMENT_ENTITY) },
        m_shaderFrame{ new Shader(PATH_VERTEX_FRAME,
            PATH_FRAGMENT_FRAME) },
        m_shaderShadow{ new Shader(PATH_VERTEX_SHADOW,
            PATH_FRAGMENT_SHADOW,
            PATH_GEOMETRY_SHADOW) },
        m_shadowMap{ new ShadowMap() },
        m_skybox{ new Skybox(PATH_VERTEX_SKYBOX,
            PATH_FRAGMENT_SKYBOX,
            PATH_TEXTURE_SKYBOX) } {
        initialize();
    }

    // initialization
    void initialize();
    void initializeAnimation();
    void initializeFrame();
    void initializeGround();
    void initializeLight();
    void initializeMaterial();
    void initializeModel();
    void initializePaths();

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
    void clampModelPosition(GLuint model);
    void clampModelScale(GLuint model);
    
    static Renderer& s_instance;
    Rendering::Primitive m_primitive{ Rendering::TRIANGLES };
    std::vector<Animation*> m_animations;
    std::vector<Joint*> m_joints;
    std::vector<LightSource*> m_lights;
    std::vector<Material*> m_materials;
    std::vector<Model*> m_models;
    std::vector<Path*> m_paths;
    std::vector<RenderedEntity*> m_entities;
    glm::mat4 m_modelMatrix;
    glm::vec3 m_modelPositions[TROOP_COUNT];
    glm::vec3 m_modelScales[TROOP_COUNT];
    Shader* m_shaderEntity;
    Shader* m_shaderFrame;
    Shader* m_shaderShadow;
    ShadowMap* m_shadowMap;
    Skybox* m_skybox;
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
    bool m_pathingEnabled{ false };
    bool m_shadowsEnabled{ true };
    bool m_texturesEnabled{ true };
};

#endif // !RENDERER_H