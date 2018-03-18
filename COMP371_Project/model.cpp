#include "model.h"

Model::ModelHierarchy* Model::getHierarchy() {
    // return model hierarchy
    return &m_hierarchy;
}

glm::mat4 Model::getJointModelMatrix(RenderedEntity* entity) {
    // return joint model matrix for entity
    glm::mat4 jointModelMatrix;
    for (Joints::iterator it{ m_joints.begin() };
        it != m_joints.end();
        ++it) {
        if ((*it)->contains(entity)) {
            jointModelMatrix = (*it)->getModelMatrix();
            break;
        }
    }

    return jointModelMatrix;
}

glm::mat4 Model::getModelMatrix(RenderedEntity* entity) {
    // initialize model matrix stack
    std::stack<glm::mat4> modelMatrixStack;

    // push hierarchy model matrices on stack until entity
    RenderedEntity* parent = entity;
    while (parent) {
        glm::mat4 jointModelMatrix = getJointModelMatrix(parent);

        modelMatrixStack.push(parent->getModelMatrixExclScaling(jointModelMatrix));
        parent = m_hierarchy.find(parent)->second;
    }

    // calculate final model matrix
    glm::mat4 modelMatrix;
    while (!modelMatrixStack.empty()) {
        modelMatrix *= modelMatrixStack.top();
        modelMatrixStack.pop();
    }

    return modelMatrix;
}

void Model::setPosition(const glm::vec3& value) {
    // set model position
    m_hierarchyRoot->setPosition(value);
}

void Model::move(Transform::Displacement direction) {
    // move model around the grid
    m_hierarchyRoot->move(direction);
}

void Model::reset() {
    // reset model hierarchy
    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->reset();

    // reset model joints
    for (Joints::iterator it{ m_joints.begin() };
        it != m_joints.end();
        ++it)
        (*it)->reset();
}

void Model::rotate(GLfloat angle,
    const glm::vec3& axis) {
    // rotate model
    m_hierarchyRoot->rotate(angle, axis);
}

void Model::rotateJoint(GLuint id,
    GLfloat angle,
    const glm::vec3& axis) {
    // rotate joint
    m_joints.at(id)->rotate(angle, axis);
}

void Model::rotateJoint(Joint* joint,
    GLfloat angle,
    const glm::vec3& axis) {
    // rotate joint
    Joints::iterator it{ std::find(m_joints.begin(),
        m_joints.end(),
        joint) };
    (*it)->rotate(angle, axis);
}

void Model::scale(const glm::vec3& value) {
    // scale model hierarchy
    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->scale(value);
}

void Model::translate(const glm::vec3& value) {
    // translate model
    m_hierarchyRoot->translate(value);
}

void Model::add(RenderedEntity* entity) {
    // add entity to hierarchy
    if (!m_hierarchyRoot)
        m_hierarchyRoot = entity;

    if (!contains(entity))
        m_hierarchy.insert(std::make_pair(entity, nullptr));
}

void Model::addJoint(Joint* joint) {
    // add joint to model
    m_joints.push_back(joint);
}

void Model::attach(RenderedEntity* toAttach,
    RenderedEntity* attachTo) {
    // attach entity to another
    add(toAttach);
    add(attachTo);
    m_hierarchy.at(toAttach) = attachTo;
}

bool Model::contains(RenderedEntity* entity) const {
    // check if hierarchy contains entity
    ModelHierarchy::const_iterator it{ m_hierarchy.find(entity) };
    if (it != m_hierarchy.end())
        return true;

    return false;
}

void Model::detach(RenderedEntity* toDetach,
    RenderedEntity* detachFrom) {
    // detach entity from another
    if (!contains(detachFrom))
        return;

    ModelHierarchy::iterator it{ m_hierarchy.find(toDetach) };
    it->second = nullptr;
}

void Model::removeJoint(Joint* joint) {
    // remove joint from model
    Joints::iterator it = std::find(m_joints.begin(), m_joints.end(), joint);
    if (it != m_joints.end())
        m_joints.erase(it);
}

void Model::setColorShaderAttributes(Shader* shader) const {
    // set shader attributes
    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->setColorShaderAttributes(shader);
}

void Model::setDepthShaderAttributes(Shader* shader) const {
    // set shader attributes
    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->setDepthShaderAttributes(shader);
}