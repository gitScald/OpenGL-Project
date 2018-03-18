#ifndef SHADOW_MAP_H
#define SHADOW_MAP_H

// project headers
#include "constants.h"
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
    GLuint getDepthTextureID() const;
    GLuint getFBOID() const;

    // utilies
    void free() const;
    void render() const;

private:
    void initialize();
    void initializeDebugQuad();

    Shader m_shaderDebug{ PATH_VERTEX_SHADOW_QUAD,
        PATH_FRAGMENT_SHADOW_QUAD };
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    GLuint m_FBO;
    GLuint m_depthTextureID;
};

#endif // !SHADOW_MAP_H