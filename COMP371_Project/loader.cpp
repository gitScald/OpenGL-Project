#include "loader.h"

/*Animation Loader::loadAnimation(const std::string& path) {
    // load animation from file
    Animation::AnimationSequence animation;
}*/

/*Path Loader::loadPath(const std::string& path) {
    // load path from file
}*/

void Loader::loadObjectData(const std::string& path,
    std::vector<glm::vec3>& outVertices,
    std::vector<glm::vec3>& outNormals,
    std::vector<glm::vec2>& outTextures) {
    // load vertices from file for use in a VBO
    std::ifstream ifs;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;
    std::vector<glm::vec2> tempTextures;
    std::vector<GLuint> indicesVertices;
    std::vector<GLuint> indicesNormals;
    std::vector<GLuint> indicesTextures;

    // enable ifstream exceptions to be thrown
    ifs.exceptions(std::ifstream::failbit
        | std::ifstream::badbit);

    // read vertices
    try {
        ifs.open(path);
        std::string line;
        while (std::getline(ifs, line)) {
            std::stringstream ss{ line };

            // vertex position
            if (line.find("v ") != std::string::npos) {
                glm::vec3 position;
                ss >> position.x >> position.y >> position.z;
                tempVertices.push_back(position);
            }

            // vertex normal
            else if (line.find("vn") != std::string::npos) {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
                tempNormals.push_back(normal);
            }

            // vertex texture
            else if (line.find("vt") != std::string::npos) {
                glm::vec3 texture;
                ss >> texture.x >> texture.y >> texture.z;
                tempTextures.push_back(texture);
            }

            // face indices
            else if (line.find("f ") != std::string::npos) {
                GLuint indexVertex[3];
                GLuint indexNormal[3];
                GLuint indexTexture[3];
                std::cout << line.c_str() << std::endl;
                sscanf_s(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
                    &indexVertex[0], &indexNormal[0], &indexTexture[0],
                    &indexVertex[1], &indexNormal[1], &indexTexture[1],
                    &indexVertex[2], &indexNormal[2], &indexTexture[2]);
                std::cout << indexVertex[0];

                for (GLuint i{ 0 }; i != 3; ++i) {
                    indicesVertices.push_back(indexVertex[i]);
                    indicesNormals.push_back(indexNormal[i]);
                    indicesTextures.push_back(indexTexture[i]);
                }
            }
        }
        std::cout << "file read";
        ifs.close();
        std::cout << "file closed";
    }
    catch (std::ifstream::failure e) {
        std::cout << ">>> Failed to read object data file: \""
            << path << "\""
            << std::endl << e.what() << std::endl;
    }

    for (GLuint i{ 0 }; i != indicesVertices.size(); ++i) {
        // triangle attribute indices
        GLuint indexVertex = indicesVertices.at(i);
        GLuint indexNormal = indicesNormals.at(i);
        GLuint indexTexture = indicesTextures.at(i);

        // triangle attributes
        glm::vec3 vertices = tempVertices.at(indexVertex - 1);
        glm::vec3 normals = tempNormals.at(indexNormal - 1);
        glm::vec2 textures = tempTextures.at(indexTexture - 1);

        // store into output vectors
        outVertices.push_back(vertices);
        outNormals.push_back(normals);
        outTextures.push_back(textures);
    }
}