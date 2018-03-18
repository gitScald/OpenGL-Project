#ifndef MODEL_H
#define MODEL_H

// project headers
#include "joint.h"

// C++ standard library headers
#include <map>
#include <stack>

class Model {
public:
    // map between entity and parent entity (if any)
    typedef std::map<RenderedEntity*, RenderedEntity*> ModelHierarchy;
    typedef std::vector<Joint*> Joints;

    Model()
        : m_position{ POSITION_ORIGIN },
        m_hierarchyRoot{ nullptr } {}
    Model(const glm::vec3& pos)
        : m_position{ pos } {}
    Model(const Model& model)
        : m_position{ model.m_position },
        m_hierarchyRoot{ model.m_hierarchyRoot },
        m_hierarchy{ model.m_hierarchy } {}
    Model(Model&& model)
        : m_position{ std::move(model.m_position) },
        m_hierarchyRoot{ std::move(model.m_hierarchyRoot) },
        m_hierarchy{ std::move(model.m_hierarchy) } {}
    Model& operator=(Model& model) = delete;

    // getters    
    ModelHierarchy* getHierarchy();
    glm::mat4 getJointModelMatrix(RenderedEntity* entity);
    glm::mat4 getModelMatrix(RenderedEntity* entity);

    // setters
    void setPosition(const glm::vec3& value);

    // transformations
    void move(Transform::Displacement direction);
    void reset();
    void rotate(GLfloat angle,
        const glm::vec3& axis);
    void rotateJoint(GLuint id,
        GLfloat angle,
        const glm::vec3& axis);
    void rotateJoint(Joint*,
        GLfloat angle,
        const glm::vec3& axis);
    void scale(const glm::vec3& value);
    void translate(const glm::vec3& value);

    // utilities
    void add(RenderedEntity* entity);
    void addJoint(Joint* joint);
    void attach(RenderedEntity* toAttach,
        RenderedEntity* attachTo);
    bool contains(RenderedEntity* entity) const;
    void detach(RenderedEntity* toDetach,
        RenderedEntity* detachFrom);
    void removeJoint(Joint* joint);
    void setColorShaderAttributes(Shader* shader) const;
    void setDepthShaderAttributes(Shader* shader) const;

private:
    RenderedEntity* m_hierarchyRoot;
    ModelHierarchy m_hierarchy;
    Joints m_joints;
    glm::vec3 m_position;
};

#endif // !MODEL_H