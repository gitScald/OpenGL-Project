#ifndef LOADER_H
#define LOADER_H

// project headers
#include "animation.h"

// GLEW
#include <gl/glew.h>

// C++ standard library headers
#include <fstream>
#include <sstream>

class Loader {
public:
    // utilities
    //Animation loadAnimation(const std::string& path);
    //Path loadPath(const std::string& path);
    static void loadObjectData(const std::string& path,
        std::vector<glm::vec3>& vertices,
        std::vector<glm::vec3>& normals,
        std::vector<glm::vec2>& textures);
};

#endif // !LOADER_H
