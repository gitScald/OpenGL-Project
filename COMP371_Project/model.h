#ifndef MODEL_H
#define MODEL_H

// project headers
#include "joint.h"

// C++ standard library headers
#include <algorithm>
#include <map>
#include <stack>

class Model {
public:
    // map between entity and parent entity (if any)
    typedef std::map<RenderedEntity*, RenderedEntity*> ModelHierarchy;
    typedef std::vector<Joint*> Joints;

    Model()
        : m_position{ POSITION_ORIGIN },
        m_hierarchyRoot{ nullptr } {
        updateVectors();
    }
    Model(const glm::vec3& pos)
        : m_position{ pos } {}
    Model(const Model& model)
        : m_position{ model.m_position },
        m_hierarchyRoot{ model.m_hierarchyRoot },
        m_hierarchy{ model.m_hierarchy },
        m_joints{ model.m_joints },
        m_front{ model.m_front },
        m_right{ model.m_right },
        m_orientation{ model.m_orientation } {}
    Model(Model&& model)
        : m_position{ std::move(model.m_position) },
        m_hierarchyRoot{ std::move(model.m_hierarchyRoot) },
        m_hierarchy{ std::move(model.m_hierarchy) },
        m_joints{ std::move(model.m_joints) },
        m_front{ std::move(m_front) },
        m_right{ std::move(m_right) },
        m_orientation{ std::move(m_orientation) } {}
    Model& operator=(Model& model) = delete;

    // getters    
    static GLfloat getSpeed();
    static GLfloat getSpeedCurrent();
    ModelHierarchy* getHierarchy();
    glm::mat4 getJointModelMatrix(RenderedEntity* entity);
    GLfloat getJointRotation(GLuint joint) const;
    glm::mat4 getModelMatrix(RenderedEntity* entity);
    GLfloat getOrientation() const;
    GLfloat getScale() const;
    GLfloat getColliderRadius() const;
    const glm::vec3& getPosition() const;

    // setters
    static void setSpeedCurrent(GLfloat value);
    void setJointRotation(GLuint joint, GLfloat angle);
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
    static void toggleSmoothMovement();
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
    void updateColliderRadius();
    void updateVectors();

    static bool s_smoothMovement;
    static GLfloat s_speed;
    static GLfloat s_speedCurrent;
    RenderedEntity* m_hierarchyRoot;
    ModelHierarchy m_hierarchy;
    Joints m_joints;
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_right;
    GLfloat m_orientation{ 0.0f };
    GLfloat m_scale{ 1.0f };
    GLfloat m_colliderRadius;
};

#endif // !MODEL_H