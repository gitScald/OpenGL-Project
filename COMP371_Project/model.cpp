#include "model.h"

// smooth movement disabled by default
bool Model::s_smoothMovement = false;

// base movement speed
GLfloat Model::s_speed = MODEL_SPEED;
GLfloat Model::s_speedCurrent = s_speed;

GLfloat Model::getSpeed() {
    // get base movement speed
    return s_speed;
}

GLfloat Model::getSpeedCurrent() {
    // get current movement speed
    return s_speedCurrent;
}

Model::ModelHierarchy* Model::getHierarchy() {
    // get model hierarchy
    return &m_hierarchy;
}

glm::mat4 Model::getJointModelMatrix(RenderedEntity* entity) {
    // get joint model matrix for entity
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

GLfloat Model::getJointRotation(GLuint joint) const {
    // get current joint rotation
    return m_joints.at(joint)->getRotation();
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

GLfloat Model::getOrientation() const {
    // get current model orientation
    return m_orientation;
}

GLfloat Model::getScale() const {
    // get current model scale
    return m_scale;
}

GLfloat Model::getColliderRadius() const {
    // return model collider radius
    return m_colliderRadius;
}

const glm::vec3& Model::getPosition() const {
    // return hierarchy root position
    return m_hierarchyRoot->getPosition();
}

void Model::setSpeedCurrent(GLfloat value) {
    // set movement speed
    s_speedCurrent = value;
    RenderedEntity::setSpeedCurrent(value);
}

void Model::setPosition(const glm::vec3& value) {
    // set model position
    m_hierarchyRoot->setPosition(value);
    m_position = value;
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

    // reset model orientation
    m_orientation = 0.0f;
    updateFrontVector();
}

void Model::rotate(GLfloat angle,
    const glm::vec3& axis) {
    // rotate model
    m_hierarchyRoot->rotate(angle, axis);
    m_orientation += angle;
    updateFrontVector();
}

void Model::rotateJoint(GLuint id,
    GLfloat angle,
    const glm::vec3& axis) {
    // rotate joint
    m_joints.at(id)->rotate(angle, axis);
    updateColliderRadius();
}

void Model::rotateJoint(Joint* joint,
    GLfloat angle,
    const glm::vec3& axis) {
    // rotate joint
    Joints::iterator it{ std::find(m_joints.begin(),
        m_joints.end(),
        joint) };
    (*it)->rotate(angle, axis);
    updateColliderRadius();
}

void Model::scale(const glm::vec3& value) {
    // scale model hierarchy
    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->scale(value);
    m_scale = value.x;
    updateColliderRadius();
}

void Model::translate(const glm::vec3& value) {
    // translate model
    m_hierarchyRoot->translate(value);
}

void Model::toggleSmoothMovement() {
    // set whether smooth movement should be enabled or not
    s_smoothMovement = !s_smoothMovement;
    RenderedEntity::toggleSmoothMovement();
}

void Model::add(RenderedEntity* entity) {
    // add entity to hierarchy
    if (!m_hierarchyRoot)
        m_hierarchyRoot = entity;

    if (!contains(entity)) {
        m_hierarchy.insert(std::make_pair(entity, nullptr));
        updateColliderRadius();
    }
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

void Model::updateColliderRadius() {
    // update radius of collider sphere
    glm::vec3 rootPosition = m_hierarchyRoot->getPosition();
    GLfloat maxX = 0;
    GLfloat maxY = 0;
    GLfloat maxZ = 0;

    for (ModelHierarchy::const_iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it) {
        if (it->first != m_hierarchyRoot) {
            // entity position relative to the root
            glm::vec3 extremity = rootPosition
                + it->first->getPosition()
                + it->first->getPivot();

            // entity dimensions
            GLfloat x = rootPosition.x - extremity.x;
            GLfloat y = rootPosition.y - extremity.y;
            GLfloat z = rootPosition.z - extremity.z;

            maxX = std::max(maxX, x);
            maxY = std::max(maxY, y);
            maxZ = std::max(maxZ, z);
        }
    }

    m_colliderRadius = std::max(maxX, maxY);
    m_colliderRadius = std::max(m_colliderRadius, maxZ);
}

void Model::updateFrontVector() {
    // calculate front vector using yaw angle
    m_front.x = cos(glm::radians(m_orientation - 90.0f));
    m_front.y = 0.0f;
    m_front.z = sin(glm::radians(m_orientation - 90.0f));

    // normalize front vector
    m_front = glm::normalize(m_front);

    // pass front vector to hierarchy
    for (ModelHierarchy::iterator it{ m_hierarchy.begin() };
        it != m_hierarchy.end();
        ++it)
        it->first->setFrontVector(m_front);
}