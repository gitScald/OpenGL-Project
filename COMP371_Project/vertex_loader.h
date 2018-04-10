#ifndef VERTEX_LOADER_H
#define VERTEX_LOADER_H

// GLEW
#include <gl/glew.h>

class VertexLoader {
public:
    VertexLoader() = delete;
    VertexLoader(const VertexLoader& loader) = delete;
    VertexLoader(VertexLoader&& loader) = delete;
    VertexLoader& operator=(VertexLoader& loader) = delete;

    // vertex loading
    static GLfloat* loadAxesVertices(GLuint* size);
    static GLfloat* loadCubeVertices(GLuint* size);
    static GLfloat* loadCubeFullVertices(GLuint* size);
    static GLfloat* loadGridVertices(GLuint* size);
    static GLfloat* loadGroundVertices(GLuint* size);

    // index loading
    static GLuint* loadGroundIndices(GLuint* size);
};

#endif // !VERTEX_LOADER_H
