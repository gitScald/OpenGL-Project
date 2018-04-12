#ifndef RENDERER_H
#define RENDERER_H

// project headers
#include "animation.h"
#include "camera.h"
#include "collision.h"
#include "constants.h"
#include "enums.h"
#include "light_source.h"
#include "material.h"
#include "model.h"
#include "particle.h"
#include "path.h"
#include "rendered_entity.h"
#include "shader.h"
#include "shadow_map.h"
#include "skybox.h"
#include "texture.h"
#include "vertex_loader.h"

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
    void toggleDayNightCycle();
    void toggleDebugging();
    void toggleFog();
    void toggleFrame();
    void toggleLights();
    void togglePathing();
    void toggleShadows();
    void toggleTextures();
    void toggleRain();
    void updateFogProperties() const;
    void updateLightPositionsAndColors();
    void updateLightProperties() const;
    void updateProjectionMatrix() const;
    void updateShadowProperties() const;
    void updateTextureProperties() const;
    void updateViewMatrix() const;

private:
    Renderer()
        : m_shaderRain{ new Shader(PATH_VERTEX_RAIN,
            PATH_FRAGMENT_RAIN) },
        m_shaderEntity { new Shader(PATH_VERTEX_ENTITY,
            PATH_FRAGMENT_ENTITY) },
        m_shaderFrame{ new Shader(PATH_VERTEX_FRAME,
            PATH_FRAGMENT_FRAME) },
        m_shaderGrass{ new Shader(PATH_VERTEX_GRASS,
            PATH_FRAGMENT_GRASS) },
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
    void initializeGrass();
    void initializeGround();
    void initializeLights();
    void initializeMaterial();
    void initializeModel();
    void initializeParticles();
    void initializePaths();

    // rendering passes
    void renderFirstPass(GLfloat deltaTime);
    void renderSecondPass(GLfloat deltaTime);

    // rendered elements
    void renderFrame();
    void renderGrass(GLfloat deltaTime, GLuint grassVersion);
    void renderGround(Shader* shader);
    void renderLights(GLfloat deltaTime);
    void renderModels(Shader* shader, GLfloat deltaTime);
    void renderParticles(GLfloat deltaTime,
        const glm::vec3& origin);

    // rendering utilities
    const glm::mat4& getWorldOrientation() const;
    glm::vec3 getWorldAxis(const glm::vec3& axis) const;

    // transformations
    void clampModelPosition(GLuint model);
    void clampModelScale(GLuint model);

    // day-night cycle
    static glm::vec4 lerpColor(const glm::vec4& start,
        const glm::vec4& end,
        GLfloat step);
    bool isDawnOrDusk() const;
    bool isDay() const;
    bool isNight() const;

    // particles
    GLint findParticle();
    void sortParticles();
    
    static Renderer& s_instance;
    Rendering::Primitive m_primitive{ Rendering::TRIANGLES };
    std::vector<Animation*> m_animations;
    std::vector<Joint*> m_joints;
    std::vector<LightSource*> m_lights;
    std::vector<Material*> m_materials;
    std::vector<Model*> m_models;
    std::vector<Path*> m_paths;
    std::vector<RenderedEntity*> m_entities;
    std::vector<glm::vec3> m_modelPositions;
    std::vector<glm::vec3> m_modelScales;
    glm::mat4 m_modelMatrix;
    glm::vec3 m_moonPosition;
    glm::vec3 m_sunPosition;
	glm::vec4 m_fogColor{ COLOR_FOG };
    glm::vec4 m_rimLightColor{ COLOR_LIGHT_DAY };
    Shader* m_shaderRain;
    Shader* m_shaderEntity;
    Shader* m_shaderFrame;
    Shader* m_shaderGrass;
    Shader* m_shaderShadow;
    ShadowMap* m_shadowMap;
    Skybox* m_skybox;
    std::vector<Particle*> m_particles;
    GLuint m_axesVAO;
    GLuint m_axesVBO;
    GLuint m_gridVAO;
    GLuint m_gridVBO;
    GLuint m_lightVAO;
    GLuint m_lightVBO;
    GLuint m_grassVAO;
    GLuint m_grassVBO;
    GLuint m_grassVBOPos;
    GLuint m_grassVBOPos2;
    GLuint m_grassEBO;
    GLuint m_particleVAO;
    GLuint m_particleVBO;
    GLuint m_particleVBOPos;
    GLuint m_particleEBO;
    GLfloat m_animationSpeed{ ANIMATION_SPEED };
    GLfloat m_animationSpeedCurrent{ ANIMATION_SPEED };
    GLfloat m_currentTime{ 0.0f };
    bool m_animationsEnabled{ false };
    bool m_dayNightCycleEnabled{ false };
    bool m_debuggingEnabled{ false };
    bool m_fogEnabled{ true };
    bool m_frameEnabled{ true };
    bool m_lightsEnabled{ true };
    bool m_pathingEnabled{ false };
    bool m_shadowsEnabled{ true };
    bool m_texturesEnabled{ true };
    bool m_rainEnabled{ false };
};

#endif // !RENDERER_H