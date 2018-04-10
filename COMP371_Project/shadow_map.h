#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

// project headers
#include "constants.h"
#include "enums.h"
#include "light_source.h"
#include "shader.h"

// GLEW
#include <gl/glew.h>

class ShadowMap {
public:
    ShadowMap() {
        initialize();
        initializeDebugQuad();
    }

    // getters
    static GLfloat getBiasMax();
    static GLfloat getBiasMin();
    static GLfloat getGridFactor();
    static GLfloat getGridOffset();
    static GLuint getGridSamples();
    GLuint getDepthTextureID() const;
    GLuint getFBOID() const;

    // utilities
    static void adjustBiasMax(Shadows::Tweak mod);
    static void adjustBiasMin(Shadows::Tweak mod);
    static void adjustGridFactor(Shadows::Tweak mod);
    static void adjustGridOffset(Shadows::Tweak mod);
    static void adjustGridSamples(Shadows::Tweak mod);
    void free() const;
    void render(LightSource* light) const;

private:
    void initialize();
    void initializeDebugQuad();

    // utilities
    static void clampBiasMax();
    static void clampBiasMin();
    static void clampGridFactor();
    static void clampGridOffset();
    static void clampGridSamples();

    static GLfloat s_biasMax;
    static GLfloat s_biasMin;
    static GLfloat s_gridFactor;
    static GLfloat s_gridOffset;
    static GLuint s_gridSamples;
    Shader m_shaderDebug{ PATH_VERTEX_SHADOW_QUAD,
        PATH_FRAGMENT_SHADOW_QUAD };
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLuint m_FBO;
    GLuint m_depthTextureID;
};

#endif // !SHADOW_MAP_H