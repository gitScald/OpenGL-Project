#include "renderer.h"

Renderer::~Renderer() {
    // free resources
    for (std::vector<Material*>::const_iterator it{ m_materials.begin() };
        it != m_materials.end();
        ++it)
        (*it)->free();
    m_shadowMap->free();

    glDeleteVertexArrays(1, &m_axesVAO);
    glDeleteBuffers(1, &m_axesVBO);
    glDeleteVertexArrays(1, &m_gridVAO);
    glDeleteBuffers(1, &m_gridVBO);
    glDeleteVertexArrays(1, &m_lightVAO);
    glDeleteBuffers(1, &m_lightVBO);
}

Renderer& Renderer::get() {
    // create or return singleton instance
    static Renderer s_instance;

    return s_instance;
}

Rendering::Primitive Renderer::getPrimitive() const {
    // return primitive
    return m_primitive;
}

GLfloat Renderer::getAnimationSpeed() const {
    // return animation speed
    return m_animationSpeed;
}

GLfloat Renderer::getAnimationSpeedCurrent() const {
    // return current animation speed
    return m_animationSpeedCurrent;
}

void Renderer::setPrimitive(Rendering::Primitive primitive) {
    // set primitive
    if (primitive == Rendering::LINES)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    m_primitive = primitive;
}

void Renderer::setAnimationSpeedCurrent(GLfloat value) {
    // set animation speed
    m_animationSpeedCurrent = value;
}

void Renderer::moveModel(GLuint model,
    Transform::Displacement direction) {
    // move model around the grid
    switch (direction) {
    case Transform::Displacement::RANDOM:
        m_modelPositions.at(model).x = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX)
            * m_models.at(model)->getScale();
        m_modelPositions.at(model).z = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX)
            * m_models.at(model)->getScale();
        break;
    case Transform::Displacement::UP:
        m_modelPositions.at(model).x += TRANSFORMATION_INCREMENT_TRANSLATION
            * m_models.at(model)->getScale();
        break;
    case Transform::Displacement::DOWN:
        m_modelPositions.at(model).x -= TRANSFORMATION_INCREMENT_TRANSLATION
            * m_models.at(model)->getScale();
        break;
    case Transform::Displacement::LEFT:
        m_modelPositions.at(model).z -= TRANSFORMATION_INCREMENT_TRANSLATION
            * m_models.at(model)->getScale();
        break;
    case Transform::Displacement::RIGHT:
        m_modelPositions.at(model).z += TRANSFORMATION_INCREMENT_TRANSLATION
            * m_models.at(model)->getScale();
        break;
    }

    // clamp position between min and max values (the grid)
    clampModelPosition(model);

    // pass position value to model
    m_models.at(model)->setPosition(glm::vec3(m_modelPositions.at(model).x
            / m_models.at(model)->getScale(),
        m_modelPositions.at(model).y,
        m_modelPositions.at(model).z
            / m_models.at(model)->getScale()));
}

void Renderer::resetModel(GLuint model) {
    // reset model
    m_modelPositions.at(model) = MODEL_POSITION_RELATIVE_TORSO;
    m_modelScales.at(model) = glm::vec3(1.0f, 1.0f, 1.0f);
    m_models.at(model)->reset();
}

void Renderer::rotateModel(GLuint model,
    Transform::Displacement direction,
    GLfloat angle) {
    // rotate model
    switch (direction) {
    case Transform::Displacement::RANDOM: {
        GLuint direction = (rand() % 2 == 0) ? -1 : 1;
        GLfloat orientation = static_cast<GLfloat>(rand()
            % static_cast<GLuint>((359.0f - 0.0f + 1.0f) - 0.0f));
        m_models.at(model)->rotate(direction * orientation,
            AXIS_Y);
    }
        break;
    case Transform::Displacement::UP:
        m_models.at(model)->rotate(angle,
            AXIS_Z);
        break;
    case Transform::Displacement::DOWN:
        m_models.at(model)->rotate(-angle,
            AXIS_Z);
        break;
    case Transform::Displacement::LEFT:
        m_models.at(model)->rotate(angle,
            AXIS_Y);
        break;
    case Transform::Displacement::RIGHT:
        m_models.at(model)->rotate(-angle,
            AXIS_Y);
        break;
    }
}

void Renderer::rotateModelJoint(GLuint model,
    GLuint id,
    Transform::Displacement direction) {
    // rotate model joint
    switch (direction) {
    case Transform::Displacement::UP:
        m_models.at(model)->rotateJoint(id,
            TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Z);
        break;
    case Transform::Displacement::DOWN:
        m_models.at(model)->rotateJoint(id,
            -TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Z);
        break;
    }
}

void Renderer::scaleModel(GLuint model,
    Transform::Scale value) {
    // scale model up or down
    switch (value) {
    case Transform::Scale::INCREASE:
        m_modelScales.at(model) += TRANSFORMATION_INCREMENT_SCALING;
        break;
    case Transform::Scale::DECREASE:
        m_modelScales.at(model) -= TRANSFORMATION_INCREMENT_SCALING;
        break;
    }

    // clamp scale between min and max values
    clampModelScale(model);

    // pass scale value to model
    m_models.at(model)->scale(m_modelScales.at(model));
}

void Renderer::render(GLfloat deltaTime) {
    renderFirstPass(deltaTime);
    renderSecondPass(deltaTime);

    // optionally render shadow map debug quad
    if (m_debuggingEnabled)
        m_shadowMap->render(m_lights.at(0));
}

void Renderer::toggleAnimations() {
    // set whether animations should be enabled or not
    m_animationsEnabled = !m_animationsEnabled;
    std::cout << "Animations: "
        << (m_animationsEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleDayNightCycle() {
    // set whether day-night cycle should be enabled or not
    m_dayNightCycleEnabled = !m_dayNightCycleEnabled;
    std::cout << "Day/night cycle: "
        << (m_dayNightCycleEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleDebugging() {
    // set whether framebuffer output should be enabled or not
    m_debuggingEnabled = !m_debuggingEnabled;
    std::cout << "Framebuffer output: "
        << (m_debuggingEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleFog() {
    // set whether fog should be enabled or not
    m_fogEnabled = !m_fogEnabled;
    std::cout << "Fog: "
        << (m_fogEnabled ? "ENABLED" : "DISABLED") << std::endl;

    // update fog properties
    updateFogProperties();
}

void Renderer::toggleFrame() {
    // set whether axes and grid should be rendered or not
    m_frameEnabled = !m_frameEnabled;
    std::cout << "Frame and grid rendering: "
        << (m_frameEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleLights() {
    // set whether lights should be enabled or not
    m_lightsEnabled = !m_lightsEnabled;
    std::cout << "Lights: "
        << (m_lightsEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::togglePathing() {
    // set whether pathing should be enabled or not
    m_pathingEnabled = !m_pathingEnabled;
    std::cout << "Pathing: "
        << (m_pathingEnabled ? "ENABLED" : "DISABLED") << std::endl;
    Model::toggleSmoothMovement();

    // also toggle animations
    if (!m_animationsEnabled)
        toggleAnimations();
}

void Renderer::toggleShadows() {
    // set whether shadows should be enabled or not
    m_shadowsEnabled = !m_shadowsEnabled;
    std::cout << "Shadows: "
        << (m_shadowsEnabled ? "ENABLED" : "DISABLED") << std::endl;

    // update shader properties
    updateShadowProperties();
}

void Renderer::toggleTextures() {
    // set whether textures should be enabled or not
    m_texturesEnabled = !m_texturesEnabled;
    Material::toggleTextures();
    std::cout << "Textures: "
        << (m_texturesEnabled ? "ENABLED" : "DISABLED") << std::endl;

    // update shader properties
    updateTextureProperties();
}

void Renderer::updateFogProperties() const {
    // update fog propeties
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformVec4(UNIFORM_FOG_COLOR,
        m_fogColor);
    m_shaderEntity->setUniformFloat(UNIFORM_FOG_DENSITY,
        FOG_DENSITY);
    m_shaderEntity->setUniformBool(UNIFORM_FOG_ENABLED,
        m_fogEnabled);

    // do so for the skybox as well
    m_skybox->updateFogProperties(m_fogEnabled);

    // and for the grass
    Shader::useProgram(m_shaderGrass->getProgramID());
    m_shaderGrass->setUniformVec4(UNIFORM_FOG_COLOR,
        m_fogColor);
    m_shaderGrass->setUniformFloat(UNIFORM_FOG_DENSITY,
        FOG_DENSITY);
    m_shaderGrass->setUniformBool(UNIFORM_FOG_ENABLED,
        m_fogEnabled);
}

void Renderer::updateLightPositionsAndColors() {
    // update light position and color in shaders
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_POSITION,
        m_lights.at(0)->getWorldPosition(
            getWorldOrientation()));
    m_shaderEntity->setUniformVec4(UNIFORM_LIGHT_COLOR,
        m_lights.at(0)->getColor());

    // do so for the skybox as well
    m_skybox->updateLightColor(m_lights.at(0)->getColor());

    // and for the grass
    Shader::useProgram(m_shaderGrass->getProgramID());
    m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_POSITION,
        m_lights.at(0)->getWorldPosition(
            getWorldOrientation()));
    m_shaderGrass->setUniformVec4(UNIFORM_LIGHT_COLOR,
        m_lights.at(0)->getColor());

    // update sun and moon positions
    if (isNight()) {
        m_moonPosition = m_lights.at(0)->getPosition();
        m_sunPosition = m_moonPosition;
        m_sunPosition.y *= -1;
        m_sunPosition.z *= -1;
    }
    else {
        m_sunPosition = m_lights.at(0)->getPosition();
        m_moonPosition = m_sunPosition;
        m_moonPosition.y *= -1;
        m_moonPosition.z *= -1;
    }

    // finally, update fog properties
    updateFogProperties();
}

void Renderer::updateLightProperties() const {
    // update shaders light properties
    Shader::useProgram(m_shaderEntity->getProgramID());
    if (m_lightsEnabled) {
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            m_lights.at(0)->getAmbient());
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            m_lights.at(0)->getDiffuse());
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            m_lights.at(0)->getSpecular());
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KC,
            m_lights.at(0)->getKC());
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KL,
            m_lights.at(0)->getKL());
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KQ,
            m_lights.at(0)->getKQ());
    }
    else {
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            glm::vec3(1.0f, 1.0f, 1.0f));
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderEntity->setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KC,
            1.0f);
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KL,
            0.0f);
        m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KQ,
            0.0f);
    }
    m_shaderEntity->setUniformFloat(UNIFORM_RIM_LIGHT_MAX,
        LIGHT_RIM_MAX);
    m_shaderEntity->setUniformFloat(UNIFORM_RIM_LIGHT_MIN,
        LIGHT_RIM_MIN);
    m_shaderEntity->setUniformVec4(UNIFORM_RIM_LIGHT_COLOR,
        m_rimLightColor);

    // and for the grass
    Shader::useProgram(m_shaderGrass->getProgramID());
    if (m_lightsEnabled) {
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            m_lights.at(0)->getAmbient());
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            m_lights.at(0)->getDiffuse());
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            m_lights.at(0)->getSpecular());
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KC,
            m_lights.at(0)->getKC());
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KL,
            m_lights.at(0)->getKL());
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KQ,
            m_lights.at(0)->getKQ());
    }
    else {
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            glm::vec3(1.0f, 1.0f, 1.0f));
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderGrass->setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KC,
            1.0f);
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KL,
            0.0f);
        m_shaderGrass->setUniformFloat(UNIFORM_LIGHT_KQ,
            0.0f);
    }
    m_shaderGrass->setUniformFloat(UNIFORM_RIM_LIGHT_MAX,
        LIGHT_RIM_MAX);
    m_shaderGrass->setUniformFloat(UNIFORM_RIM_LIGHT_MIN,
        LIGHT_RIM_MIN);
    m_shaderGrass->setUniformVec4(UNIFORM_RIM_LIGHT_COLOR,
        m_rimLightColor);

    // and for the particles
    Shader::useProgram(m_shaderBlade->getProgramID());
    m_shaderBlade->setUniformVec4(UNIFORM_RIM_LIGHT_COLOR,
        m_rimLightColor);
}

void Renderer::updateProjectionMatrix() const {
    // update shaders projection matrix
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());

    Shader::useProgram(m_shaderFrame->getProgramID());
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());

    // update skybox projection matrix
    m_skybox->updateProjectionMatrix(Camera::get().getProjectionMatrix());
}

void Renderer::updateShadowProperties() const {
    // update shader properties
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformBool(UNIFORM_SHADOWS_ENABLED,
        m_shadowsEnabled);
    m_shaderEntity->setUniformUInt(UNIFORM_SHADOW_GRID_SAMPLES,
        ShadowMap::getGridSamples());
    m_shaderEntity->setUniformFloat(UNIFORM_SHADOW_GRID_OFFSET,
        ShadowMap::getGridOffset());
    m_shaderEntity->setUniformFloat(UNIFORM_SHADOW_GRID_FACTOR,
        ShadowMap::getGridFactor());
    m_shaderEntity->setUniformFloat(UNIFORM_SHADOW_BIAS_MIN,
        ShadowMap::getBiasMin());
    m_shaderEntity->setUniformFloat(UNIFORM_SHADOW_BIAS_MAX,
        ShadowMap::getBiasMax());
    m_shaderEntity->setUniformUInt(UNIFORM_SHADOW_DEPTH_TEXTURE,
        TEXTURE_INDEX_DEPTH_MAP);
}

void Renderer::updateTextureProperties() const {
    // update shader properties
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformBool(UNIFORM_TEXTURES_ENABLED,
        m_texturesEnabled);
}

void Renderer::updateViewMatrix() const {
    // update shaders view matrix and camera position
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderEntity->setUniformVec3(UNIFORM_CAMERA_POSITION,
        Camera::get().getPosition());

    Shader::useProgram(m_shaderFrame->getProgramID());
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());

    // update skybox view matrix
    m_skybox->updateViewMatrix(Camera::get().getViewMatrix());
}

void Renderer::initialize() {
    // initialize random seed
    srand(static_cast<GLuint>(time(NULL)));

    // initialize model positions and scales
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i) {
        m_modelPositions.push_back(MODEL_POSITION_RELATIVE_TORSO);
        m_modelScales.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // reserve capacity for entities vector
    m_entities.reserve(1 + TROOP_COUNT * 11);

    // reserve capacity for joints vector
    m_joints.reserve(TROOP_COUNT * 10);

    // reserve capacity for paths vector
    m_paths.reserve(TROOP_COUNT);

    // call separate initialization methods
    initializeFrame();
    initializeGround();
    initializeGrass();
    initializeParticles();
    initializeMaterial();
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i) {
        initializeAnimation();
        initializeModel();
    }
    initializePaths();
    initializeLights();

    // set initial view and projection matrices
    updateViewMatrix();
    updateProjectionMatrix();

    // set initial light properties
    updateLightPositionsAndColors();
    updateLightProperties();

    // set initial states for fog, shadows and textures
    updateFogProperties();
    updateShadowProperties();
    updateTextureProperties();
}

void Renderer::initializeAnimation() {
    // create animation
    Animation* animation = new Animation();

    // head keyframes
    animation->addStep(new AnimationStep(JOINT_HEAD,
        AXIS_Z, 0, 15.0f));
    animation->addStep(new AnimationStep(JOINT_HEAD,
        AXIS_Z, 1, -5.0f));
    animation->addStep(new AnimationStep(JOINT_HEAD,
        AXIS_Z, 2, 5.0f));
    animation->addStep(new AnimationStep(JOINT_HEAD,
        AXIS_Z, 3, -5.0f));

    // neck keyframes
    animation->addStep(new AnimationStep(JOINT_NECK,
        AXIS_Z, 0, 10.0f));
    animation->addStep(new AnimationStep(JOINT_NECK,
        AXIS_Z, 1, 10.0f));
    animation->addStep(new AnimationStep(JOINT_NECK,
        AXIS_Z, 2, -5.0f));
    animation->addStep(new AnimationStep(JOINT_NECK,
        AXIS_Z, 3, -10.0f));

    // leg upper front right keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_RIGHT,
        AXIS_Z, 0, 35.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_RIGHT,
        AXIS_Z, 1, -40.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_RIGHT,
        AXIS_Z, 2, 80.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_RIGHT,
        AXIS_Z, 3, -20.0f));

    // leg lower front right keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_RIGHT,
        AXIS_Z, 0, -20.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_RIGHT,
        AXIS_Z, 1, -15.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_RIGHT,
        AXIS_Z, 2, -70.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_RIGHT,
        AXIS_Z, 3, 40.0f));

    // leg upper back right keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_RIGHT,
        AXIS_Z, 0, -25.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_RIGHT,
        AXIS_Z, 1, -65.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_RIGHT,
        AXIS_Z, 2, 20.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_RIGHT,
        AXIS_Z, 3, -15.0f));

    // leg lower back right keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_RIGHT,
        AXIS_Z, 0, 15.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_RIGHT,
        AXIS_Z, 1, 25.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_RIGHT,
        AXIS_Z, 2, 40.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_RIGHT,
        AXIS_Z, 3, -10.0f));

    // leg upper front left keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_LEFT,
        AXIS_Z, 0, 40.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_LEFT,
        AXIS_Z, 1, -35.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_LEFT,
        AXIS_Z, 2, 65.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_FRONT_LEFT,
        AXIS_Z, 3, -25.0f));

    // leg lower front left keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_LEFT,
        AXIS_Z, 0, -25.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_LEFT,
        AXIS_Z, 1, -20.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_LEFT,
        AXIS_Z, 2, -55.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_FRONT_LEFT,
        AXIS_Z, 3, 45.0f));

    // leg upper back left keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_LEFT,
        AXIS_Z, 0, -30.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_LEFT,
        AXIS_Z, 1, -70.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_LEFT,
        AXIS_Z, 2, 65.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_UPPER_BACK_LEFT,
        AXIS_Z, 3, -25.0f));

    // leg lower back left keyframes
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_LEFT,
        AXIS_Z, 0, 20.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_LEFT,
        AXIS_Z, 1, 30.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_LEFT,
        AXIS_Z, 2, 35.0f));
    animation->addStep(new AnimationStep(JOINT_LEG_LOWER_BACK_LEFT,
        AXIS_Z, 3, -15.0f));

    // torso keyframes
    animation->addStep(new AnimationStep(JOINT_TORSO,
        AXIS_Z, 0, -5.0f));
    animation->addStep(new AnimationStep(JOINT_TORSO,
        AXIS_Z, 1, 10.0f));
    animation->addStep(new AnimationStep(JOINT_TORSO,
        AXIS_Z, 2, -10.0f));
    animation->addStep(new AnimationStep(JOINT_TORSO,
        AXIS_Z, 3, 15.0f));

    // add animation to animations vector
    m_animations.push_back(animation);
}

void Renderer::initializeFrame() {
    // axes vertex data
    GLuint verticesSize;
    GLfloat* verticesAxes = VertexLoader::loadAxesVertices(&verticesSize);

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_axesVAO);
    Shader::bindVAO(m_axesVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_axesVBO);
    Shader::bindVBO(m_axesVBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        verticesAxes,
        GL_STATIC_DRAW);
    delete[] verticesAxes;

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shaderFrame->getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);

    // grid vertex data
    GLfloat* verticesGrid = VertexLoader::loadGridVertices(&verticesSize);

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_gridVAO);
    Shader::bindVAO(m_gridVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_gridVBO);
    Shader::bindVBO(m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        verticesGrid,
        GL_STATIC_DRAW);
    delete[] verticesGrid;

    // vertex attributes
    positionLocation = glGetAttribLocation(m_shaderFrame->getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);
}

void Renderer::initializeGrass() {
    glm::vec3 offset[GRASS_COUNT];
    for (GLuint i{ 0 }; i != GRASS_COUNT; ++i) {
        offset[i].x = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
        offset[i].y = 0.0f;
        offset[i].z = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
    }

    glGenBuffers(1, &m_grassVBOPos);
    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBOPos);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * GRASS_COUNT,
        &offset[0],
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    offset[GRASS_COUNT];
    for (GLuint i{ 0 }; i != GRASS_COUNT; ++i) {
        offset[i].x = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
        offset[i].y = 0.0f;
        offset[i].z = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
    }

    glGenBuffers(1, &m_grassVBOPos2);
    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBOPos2);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * GRASS_COUNT,
        &offset[0],
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vertex data
    GLuint verticesSize;
    GLfloat* verticesGrass = VertexLoader::loadGrassVertices(&verticesSize);

    // buffers
    glGenVertexArrays(1, &m_grassVAO);
    glBindVertexArray(m_grassVAO);

    glGenBuffers(1, &m_grassVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        verticesGrass,
        GL_STATIC_DRAW);
    delete[] verticesGrass;

    GLuint indicesSize;
    GLuint* indicesGrass = VertexLoader::loadGrassIndices(&indicesSize);

    glGenBuffers(1, &m_grassEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_grassEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indicesSize,
        indicesGrass,
        GL_STATIC_DRAW);
    delete[] indicesGrass;

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBOPos);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glVertexAttribDivisor(3, 1);

    // initialize grass materials
    Shader::useProgram(m_shaderGrass->getProgramID());
    m_shaderGrass->setUniformVec4(UNIFORM_COLOR,
        COLOR_GRASS);
    m_materials.push_back(new Material(
        Texture(PATH_TEXTURE_GRASS_0,
            GL_RGBA,
            GL_RGBA,
            GL_REPEAT,
            GL_LINEAR).getID(),
        MATERIAL_SHININESS_GRASS));
    m_materials.push_back(new Material(
        Texture(PATH_TEXTURE_GRASS_1,
            GL_RGBA,
            GL_RGBA,
            GL_REPEAT,
            GL_LINEAR).getID(),
        MATERIAL_SHININESS_GRASS));

    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBOPos2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glVertexAttribDivisor(3, 1);
}

void Renderer::initializeGround() {
    // vertex data
    GLuint verticesSize;
    GLfloat* verticesGround = VertexLoader::loadGroundVertices(&verticesSize);

    GLuint indicesSize;
    GLuint* indicesGround = VertexLoader::loadGroundIndices(&indicesSize);

    // initialize ground material
    m_materials.push_back(new Material(
        Texture(PATH_TEXTURE_GROUND,
            GL_RGB,
            GL_RGB,
            GL_REPEAT,
            GL_LINEAR).getID(),
            MATERIAL_SHININESS_GROUND));

    // add ground entity to entities vector
    m_entities.push_back(new RenderedEntity(
        m_shaderEntity,
        POSITION_ORIGIN,
        POSITION_ORIGIN,
        verticesGround,
        verticesSize,
        indicesGround,
        indicesSize));
    delete[] verticesGround;
    delete[] indicesGround;

    // set ground color
    m_entities.at(0)->setColor(COLOR_GROUND);
}

void Renderer::initializeLights() {
    // vertex data
    GLuint verticesSize;
    GLfloat* vertices = VertexLoader::loadCubeVertices(&verticesSize);

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_lightVAO);
    Shader::bindVAO(m_lightVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_lightVBO);
    Shader::bindVBO(m_lightVBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        vertices,
        GL_STATIC_DRAW);
    delete[] vertices;

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shaderFrame->getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);

    // create light source and add it to lights vector
    m_lights.push_back(new LightSource(LIGHT_POSITION_NOON, COLOR_LIGHT_DAY));

    // set shader uniforms
    Shader::useProgram(m_shaderEntity->getProgramID());
    m_shaderEntity->setUniformVec2(UNIFORM_LIGHT_PLANES,
        glm::vec2(m_lights.at(0)->getPlaneNear(),
            m_lights.at(0)->getPlaneFar()));
    m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KC,
        m_lights.at(0)->getKC());
    m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KL,
        m_lights.at(0)->getKL());
    m_shaderEntity->setUniformFloat(UNIFORM_LIGHT_KQ,
        m_lights.at(0)->getKQ());

    Shader::useProgram(m_shaderShadow->getProgramID());
    m_shaderShadow->setUniformVec2(UNIFORM_LIGHT_PLANES,
        glm::vec2(m_lights.at(0)->getPlaneNear(),
            m_lights.at(0)->getPlaneFar()));
}

void Renderer::initializeMaterial() {
    // initialize model material
    m_materials.push_back(new Material(
        Texture(PATH_TEXTURE_HORSE,
            GL_RGB,
            GL_RGB,
            GL_CLAMP_TO_EDGE,
            GL_LINEAR).getID(),
            MATERIAL_SHININESS_HORSE));
}

void Renderer::initializeModel() {
    // vertex data
    GLuint verticesSize;
    GLfloat* vertices = VertexLoader::loadCubeFullVertices(&verticesSize);

    // create model entities
    RenderedEntity* head = new RenderedEntity(m_shaderEntity,
        POSITION_ORIGIN,
        0.5f * MODEL_POSITION_RELATIVE_HEAD,
        vertices,
        verticesSize,
        nullptr,
        NULL,
        true);
    delete[] vertices;
    RenderedEntity* legLowerFrontRight = new RenderedEntity(*head);
    RenderedEntity* legLowerFrontLeft = new RenderedEntity(*head);
    RenderedEntity* legLowerBackRight = new RenderedEntity(*head);
    RenderedEntity* legLowerBackLeft = new RenderedEntity(*head);
    RenderedEntity* legUpperFrontRight = new RenderedEntity(*head);
    RenderedEntity* legUpperFrontLeft = new RenderedEntity(*head);
    RenderedEntity* legUpperBackRight = new RenderedEntity(*head);
    RenderedEntity* legUpperBackLeft = new RenderedEntity(*head);
    RenderedEntity* neck = new RenderedEntity(*head);
    RenderedEntity* torso = new RenderedEntity(*head);

    // set body parts joint pivot points
    head->setPivot(0.5f * MODEL_POSITION_RELATIVE_HEAD);
    legLowerFrontRight->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_FR);
    legLowerFrontLeft->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_FL);
    legLowerBackRight->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_BR);
    legLowerBackLeft->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_BL);
    legUpperFrontRight->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_UPPER_FR);
    legUpperFrontLeft->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_UPPER_FL);
    legUpperBackRight->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_UPPER_BR);
    legUpperBackLeft->setPivot(0.5f * MODEL_POSITION_RELATIVE_LEG_UPPER_BL);
    neck->setPivot(0.5f * MODEL_POSITION_RELATIVE_NECK);
    torso->setPivot(0.5f * MODEL_POSITION_RELATIVE_TORSO);

    // set body parts color
    head->setColor(MODEL_COLOR_HEAD);
    legLowerFrontRight->setColor(MODEL_COLOR_LEG_LOWER);
    legLowerFrontLeft->setColor(MODEL_COLOR_LEG_LOWER);
    legLowerBackRight->setColor(MODEL_COLOR_LEG_LOWER);
    legLowerBackLeft->setColor(MODEL_COLOR_LEG_LOWER);
    legUpperFrontRight->setColor(MODEL_COLOR_LEG_UPPER);
    legUpperFrontLeft->setColor(MODEL_COLOR_LEG_UPPER);
    legUpperBackRight->setColor(MODEL_COLOR_LEG_UPPER);
    legUpperBackLeft->setColor(MODEL_COLOR_LEG_UPPER);
    neck->setColor(MODEL_COLOR_NECK);
    torso->setColor(MODEL_COLOR_TORSO);

    // scale body parts
    head->scale(MODEL_SCALE_HEAD);
    legLowerFrontRight->scale(MODEL_SCALE_LEG_LOWER);
    legLowerFrontLeft->scale(MODEL_SCALE_LEG_LOWER);
    legLowerBackRight->scale(MODEL_SCALE_LEG_LOWER);
    legLowerBackLeft->scale(MODEL_SCALE_LEG_LOWER);
    legUpperFrontRight->scale(MODEL_SCALE_LEG_UPPER);
    legUpperFrontLeft->scale(MODEL_SCALE_LEG_UPPER);
    legUpperBackRight->scale(MODEL_SCALE_LEG_UPPER);
    legUpperBackLeft->scale(MODEL_SCALE_LEG_UPPER);
    neck->scale(MODEL_SCALE_NECK);
    torso->scale(MODEL_SCALE_TORSO);

    // translate body parts to their appropriate locations
    head->translate(MODEL_POSITION_RELATIVE_HEAD);
    legLowerFrontRight->translate(MODEL_POSITION_RELATIVE_LEG_LOWER_FR);
    legLowerFrontLeft->translate(MODEL_POSITION_RELATIVE_LEG_LOWER_FL);
    legLowerBackRight->translate(MODEL_POSITION_RELATIVE_LEG_LOWER_BR);
    legLowerBackLeft->translate(MODEL_POSITION_RELATIVE_LEG_LOWER_BL);
    legUpperFrontRight->translate(MODEL_POSITION_RELATIVE_LEG_UPPER_FR);
    legUpperFrontLeft->translate(MODEL_POSITION_RELATIVE_LEG_UPPER_FL);
    legUpperBackRight->translate(MODEL_POSITION_RELATIVE_LEG_UPPER_BR);
    legUpperBackLeft->translate(MODEL_POSITION_RELATIVE_LEG_UPPER_BL);
    neck->translate(MODEL_POSITION_RELATIVE_NECK);
    torso->translate(MODEL_POSITION_RELATIVE_TORSO);

    // rotate head and neck, set torso rotation
    head->rotate(MODEL_ORIENTATION_RELATIVE_HEAD.first,
        MODEL_ORIENTATION_RELATIVE_HEAD.second);
    neck->rotate(MODEL_ORIENTATION_RELATIVE_NECK.first,
        MODEL_ORIENTATION_RELATIVE_NECK.second);
    torso->lockRotation(true);

    // add body parts to entities vector
    m_entities.push_back(head);
    m_entities.push_back(legLowerFrontRight);
    m_entities.push_back(legLowerFrontLeft);
    m_entities.push_back(legLowerBackRight);
    m_entities.push_back(legLowerBackLeft);
    m_entities.push_back(legUpperFrontRight);
    m_entities.push_back(legUpperFrontLeft);
    m_entities.push_back(legUpperBackRight);
    m_entities.push_back(legUpperBackLeft);
    m_entities.push_back(neck);
    m_entities.push_back(torso);

    // create model object
    m_models.push_back(new Model());
    GLuint modelIndex = m_models.size() - 1;

    // get indices of above entities
    GLuint entitiesHead = modelIndex * 11 + 1;
    GLuint entitiesLegLowerFrontRight = entitiesHead + 1;
    GLuint entitiesLegLowerFrontLeft = entitiesHead + 2;
    GLuint entitiesLegLowerBackRight = entitiesHead + 3;
    GLuint entitiesLegLowerBackLeft = entitiesHead + 4;
    GLuint entitiesLegUpperFrontRight = entitiesHead + 5;
    GLuint entitiesLegUpperFrontLeft = entitiesHead + 6;
    GLuint entitiesLegUpperBackRight = entitiesHead + 7;
    GLuint entitiesLegUpperBackLeft = entitiesHead + 8;
    GLuint entitiesNeck = entitiesHead + 9;
    GLuint entitiesTorso = entitiesHead + 10;

    // set up model hierarchy
    m_models.at(modelIndex)->add(m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesNeck),
        m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesHead),
        m_entities.at(entitiesNeck));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegUpperFrontRight),
        m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegUpperFrontLeft),
        m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegUpperBackRight),
        m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegUpperBackLeft),
        m_entities.at(entitiesTorso));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegLowerFrontRight),
        m_entities.at(entitiesLegUpperFrontRight));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegLowerFrontLeft),
        m_entities.at(entitiesLegUpperFrontLeft));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegLowerBackRight),
        m_entities.at(entitiesLegUpperBackRight));
    m_models.at(modelIndex)->attach(m_entities.at(entitiesLegLowerBackLeft),
        m_entities.at(entitiesLegUpperBackLeft));

    // create model joints
    Joint* joint0 = new Joint(m_entities.at(entitiesNeck),
        m_entities.at(entitiesHead),
        m_entities.at(entitiesHead)->getPivot(),
        MODEL_ROTATION_HEAD_MAX,
        MODEL_ROTATION_HEAD_MIN);
    Joint* joint1 = new Joint(m_entities.at(entitiesTorso),
        m_entities.at(entitiesNeck),
        m_entities.at(entitiesNeck)->getPivot(),
        MODEL_ROTATION_NECK_MAX,
        MODEL_ROTATION_NECK_MIN);
    Joint* joint2 = new Joint(m_entities.at(entitiesTorso),
        m_entities.at(entitiesLegUpperFrontRight),
        MODEL_SCALE_LEG_UPPER
        * m_entities.at(entitiesLegUpperFrontRight)->getPivot(),
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint* joint3 = new Joint(m_entities.at(entitiesLegUpperFrontRight),
        m_entities.at(entitiesLegLowerFrontRight),
        m_entities.at(entitiesLegLowerFrontRight)->getPivot(),
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint* joint4 = new Joint(m_entities.at(entitiesTorso),
        m_entities.at(entitiesLegUpperBackRight),
        MODEL_SCALE_LEG_UPPER
        * m_entities.at(entitiesLegUpperBackRight)->getPivot(),
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint* joint5 = new Joint(m_entities.at(entitiesLegUpperBackRight),
        m_entities.at(entitiesLegLowerBackRight),
        m_entities.at(entitiesLegLowerBackRight)->getPivot(),
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint* joint6 = new Joint(m_entities.at(entitiesTorso),
        m_entities.at(entitiesLegUpperFrontLeft),
        MODEL_SCALE_LEG_UPPER
        * m_entities.at(entitiesLegUpperFrontLeft)->getPivot(),
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint* joint7 = new Joint(m_entities.at(entitiesLegUpperFrontLeft),
        m_entities.at(entitiesLegLowerFrontLeft),
        m_entities.at(entitiesLegLowerFrontLeft)->getPivot(),
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint* joint8 = new Joint(m_entities.at(entitiesTorso),
        m_entities.at(entitiesLegUpperBackLeft),
        MODEL_SCALE_LEG_UPPER
        * m_entities.at(entitiesLegUpperBackLeft)->getPivot(),
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint* joint9 = new Joint(m_entities.at(entitiesLegUpperBackLeft),
        m_entities.at(entitiesLegLowerBackLeft),
        m_entities.at(entitiesLegLowerBackLeft)->getPivot(),
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint* joint10 = new Joint(nullptr,
        m_entities.at(entitiesTorso),
        m_entities.at(entitiesTorso)->getPivot(),
        MODEL_ROTATION_TORSO_MAX,
        MODEL_ROTATION_TORSO_MIN);

    // add model joints to joints vector
    m_joints.push_back(joint0);
    m_joints.push_back(joint1);
    m_joints.push_back(joint2);
    m_joints.push_back(joint3);
    m_joints.push_back(joint4);
    m_joints.push_back(joint5);
    m_joints.push_back(joint6);
    m_joints.push_back(joint7);
    m_joints.push_back(joint8);
    m_joints.push_back(joint9);
    m_joints.push_back(joint10);

    // add joints to model
    GLuint jointStart = modelIndex * 11;
    GLuint jointEnd = jointStart + 11;
    for (GLuint j{ jointStart }; j != jointEnd; ++j)
        m_models.at(modelIndex)->addJoint(m_joints.at(j));

    // scale model
    for (GLuint i{ 0 }; i != 5; ++i)
        scaleModel(modelIndex, Transform::INCREASE);

    // place horse at random location and rotate it randomly
    if (modelIndex != 0) {
        moveModel(modelIndex, Transform::RANDOM);
        rotateModel(modelIndex, Transform::RANDOM);
    }
}

void Renderer::initializeParticles() {
    for (GLuint i{ 0 }; i != PARTICLE_COUNT; ++i) {
        m_particles[i].m_life = -1.0f;
        m_particles[i].m_distanceToCamera = -1.0f;
    }

    // vertex data
    GLuint verticesSize;
    GLfloat* verticesParticle = VertexLoader::loadParticleVertices(&verticesSize);

    // buffers
    glGenVertexArrays(1, &m_particleVAO);
    glBindVertexArray(m_particleVAO);

    glGenBuffers(1, &m_particleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
    glBufferData(GL_ARRAY_BUFFER,
        verticesSize,
        verticesParticle,
        GL_STREAM_DRAW);
    delete[] verticesParticle;

    GLuint indicesSize;
    GLuint* indicesParticle = VertexLoader::loadParticleIndices(&indicesSize);

    glGenBuffers(1, &m_particleEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_particleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indicesSize,
        indicesParticle,
        GL_STREAM_DRAW);
    delete[] indicesParticle;

    glGenBuffers(1, &m_particleVBOPos);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBOPos);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * PARTICLE_COUNT,
        NULL,
        GL_STREAM_DRAW);

    m_materials.push_back(new Material(
        Texture(PATH_TEXTURE_BLADE,
            GL_RGBA,
            GL_RGBA,
            GL_REPEAT,
            GL_LINEAR).getID(),
        MATERIAL_SHININESS_GRASS));
}

void Renderer::initializePaths() {
    // create a sequence of random paths for each horse to follow
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i) {
        m_paths.push_back(new Path());

        for (GLuint j{ 0 }; j != PATH_COUNT; ++j) {
            GLfloat orientation = ((rand() % 2 == 0) ? -1 : 1)
                * (rand()
                    % static_cast<GLuint>(
                        (PATHING_DIRECTION_MAX - PATHING_DIRECTION_MIN + 1))
                    + PATHING_DIRECTION_MIN);
            //GLfloat orientation = 45.0f;
            GLfloat travelTime = (rand()
                % static_cast<GLuint>(
                    (PATHING_DISTANCE_MAX - PATHING_DISTANCE_MIN + 1))
                + PATHING_DISTANCE_MIN);

            m_paths.at(i)->addStep(new PathStep(j,
                orientation,
                travelTime));
        }
    }
}

void Renderer::renderFirstPass(GLfloat deltaTime) {
    // compute depth texture transformation matrices
    glm::mat4 shadowProjection = glm::perspective(
        glm::radians(SHADOW_PROJECTION_FOV),
        SHADOW_ASPECT_RATIO,
        m_lights.at(0)->getPlaneNear(),
        m_lights.at(0)->getPlaneFar());
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0)->getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0)->getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));

    // set viewport to depth texture dimensions
    glViewport(0,
        0,
        SHADOW_DEPTH_TEXTURE_WIDTH,
        SHADOW_DEPTH_TEXTURE_HEIGHT);

    // bind and clear shadow map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMap->getFBOID());
    glClear(GL_DEPTH_BUFFER_BIT);

    // set shadow map shader uniforms
    Shader::useProgram(m_shaderShadow->getProgramID());
    for (GLuint i{ 0 }; i != shadowTransforms.size(); ++i)
        m_shaderShadow->setUniformMat4(UNIFORM_SHADOW_TRANSFORMS
            + "[" + std::to_string(i) + "]",
            shadowTransforms[i]);
    m_shaderShadow->setUniformVec3(UNIFORM_LIGHT_POSITION,
        m_lights.at(0)->getWorldPosition(
            getWorldOrientation()));

    // render ground to depth texture
    renderGround(m_shaderShadow);

    // render models to depth texture
    renderModels(m_shaderShadow, deltaTime);

    // unbind shadow map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
}

void Renderer::renderSecondPass(GLfloat deltaTime) {
    // set viewport back to window dimensions
    glViewport(0,
        0,
        Camera::get().getViewportWidth(),
        Camera::get().getViewportHeight());

    // clear buffer
    glClearColor(COLOR_CLEAR.r, COLOR_CLEAR.g, COLOR_CLEAR.b, COLOR_CLEAR.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render skybox
    if (m_texturesEnabled)
    m_skybox->render(Camera::get().getWorldOrientation(),
        Camera::get().getPosition());

    // shader uniforms: lighting
    Shader::useProgram(m_shaderEntity->getProgramID());

    // render ground
    m_materials.at(0)->use(m_shaderEntity);
    Shader::activateTextureUnit(TEXTURE_UNIT_DEPTH_MAP);
    Shader::bindCubemapTexture(m_shadowMap->getDepthTextureID());
    renderGround(m_shaderEntity);

    // render models
    m_materials.at(4)->use(m_shaderEntity);
    renderModels(m_shaderEntity, deltaTime);

    // render grass
    m_materials.at(1)->use(m_shaderGrass);
    renderGrass(deltaTime, 0);
    m_materials.at(2)->use(m_shaderGrass);
    renderGrass(deltaTime, 1);

    // render particles
    m_materials.at(3)->use(m_shaderBlade);
    renderParticles(deltaTime, POSITION_ORIGIN);

    // render light
    renderLights(deltaTime);

    // render axes and grid
    if (m_frameEnabled)
        renderFrame();
}

void Renderer::renderFrame() {
    // set line width
    glLineWidth(RENDERING_LINE_WIDTH);

    // set shader uniforms
    Shader::useProgram(m_shaderFrame->getProgramID());
    Shader::bindVAO(m_axesVAO);
    for (GLuint i{ 0 }; i != 3; ++i) {
        // set model matrix and color
        glm::mat4 modelMatrix;
        modelMatrix *= getWorldOrientation()
            * glm::rotate(modelMatrix,
                glm::radians(ROTATION_AXES[i].first),
                ROTATION_AXES[i].second);
        m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_MODEL,
            modelMatrix);
        m_shaderFrame->setUniformVec4(UNIFORM_COLOR,
            COLOR_AXES[i]);

        // render axes
        glDrawArrays(GL_LINES, 0, 2);
    }

    // set model uniforms
    glm::mat4 modelMatrix = getWorldOrientation();
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_MODEL,
        modelMatrix);
    m_shaderFrame->setUniformVec4(UNIFORM_COLOR,
        COLOR_GRID);

    // render grid
    Shader::bindVAO(m_gridVAO);
    modelMatrix = getWorldOrientation();
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_MODEL,
        modelMatrix);
    glDrawArrays(GL_LINES, 0, 404);

    // revert line width to normal
    glLineWidth(1.0f);
}

void Renderer::renderGrass(GLfloat deltaTime,
    GLuint grassVersion) {
    Shader::useProgram(m_shaderGrass->getProgramID());
    m_shaderGrass->setUniformMat4(UNIFORM_MATRIX_MODEL,
        getWorldOrientation());
    m_shaderGrass->setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderGrass->setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());
    m_shaderGrass->setUniformFloat(UNIFORM_WIND_TIME,
        m_currentTime);
    m_shaderGrass->setUniformFloat(UNIFORM_WIND_STRENGTH,
        WIND_STRENGTH * sin(m_currentTime));
    m_shaderGrass->setUniformVec3(UNIFORM_WIND_DIRECTION,
        glm::normalize(
            glm::vec3(sin(m_currentTime), 0.0f, cos(m_currentTime))));

    glBindVertexArray(m_grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_grassVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_grassEBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (void*)(6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, grassVersion == 0
        ? m_grassVBOPos
        : m_grassVBOPos2);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
    glVertexAttribDivisor(3, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP,
        0,
        12,
        GRASS_COUNT);
}

void Renderer::renderGround(Shader* shader) {
    // set shader attributes
    if (shader == m_shaderShadow)
        m_entities.at(0)->setDepthShaderAttributes(shader);
    else if (shader == m_shaderEntity)
        m_entities.at(0)->setColorShaderAttributes(shader);

    // pass model matrix to shader and render
    glm::mat4 modelMatrix = m_entities.at(0)->getModelMatrix(
        getWorldOrientation());
    shader->setUniformMat4(UNIFORM_MATRIX_MODEL, modelMatrix);
    m_entities.at(0)->render(m_primitive);
}

void Renderer::renderLights(GLfloat deltaTime) {
    // handle day-night cycle
    if (m_dayNightCycleEnabled) {
        // increment time of day
        m_currentTime += LIGHT_SPEED * deltaTime;

        // update light position
        glm::vec3 currentPosition = m_lights.at(0)->getPosition();
        GLfloat nightTime = isNight() ? -1.0f : 1.0f;
        GLfloat y = LIGHT_POSITION_NOON.y * sin(nightTime * m_currentTime);
        GLfloat z = LIGHT_POSITION_NOON.y * nightTime * cos(m_currentTime);
        glm::vec3 newPosition{ currentPosition.x, y, z };
        m_lights.at(0)->setPosition(newPosition);
        
        // update light color
        glm::vec4 currentColor = m_lights.at(0)->getColor();
        glm::vec4 targetColor;

        // night time
        if (isNight())
            targetColor = COLOR_LIGHT_NIGHT;

        // day time
        else if (isDay())
            targetColor = COLOR_LIGHT_DAY;

        // dawn or dusk time
        else
            targetColor = COLOR_LIGHT_TRANSITION;

        // lerp color and assign to light and rim lighting
        glm::vec4 newColor{ lerpColor(currentColor, targetColor, 0.1f) };
        m_lights.at(0)->setColor(newColor);
        m_rimLightColor = newColor;

		// lerp between fog color and rim lighting color
		m_fogColor = lerpColor(m_fogColor, m_rimLightColor, 0.6f);

        // update shader properties
        updateLightPositionsAndColors();

        // reset timer after a full cycle
        if (m_currentTime >= 2.0 * glm::pi<GLfloat>())
            m_currentTime = 0.0f;
    }

    // set shader uniforms for sun
    Shader::useProgram(m_shaderFrame->getProgramID());
    glm::mat4 modelMatrix = getWorldOrientation()
        * glm::translate(glm::mat4(), m_sunPosition)
        * glm::scale(glm::mat4(), LIGHT_SCALE);
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_MODEL, modelMatrix);
    m_shaderFrame->setUniformVec4(UNIFORM_COLOR,
        m_lightsEnabled
        ? COLOR_LIGHT_SUN_ON
        : COLOR_LIGHT_SUN_OFF);

    // render sun
    Shader::bindVAO(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // set shader uniforms for moon
    modelMatrix = getWorldOrientation()
        * glm::translate(glm::mat4(), m_moonPosition)
        * glm::scale(glm::mat4(), LIGHT_SCALE);
    m_shaderFrame->setUniformMat4(UNIFORM_MATRIX_MODEL, modelMatrix);
    m_shaderFrame->setUniformVec4(UNIFORM_COLOR,
        m_lightsEnabled
        ? COLOR_LIGHT_MOON_ON
        : COLOR_LIGHT_MOON_OFF);

    // render moon
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::renderModels(Shader* shader, GLfloat deltaTime) {
    // cull front faces to limit peter panning
    glCullFace(GL_FRONT);

    // collision detection
    std::vector<Model*> collidingModels
        = Collision::detectCollisions(m_models);

    // render models
    GLuint modelIndex{ 0 };
    for (std::vector<Model*>::iterator m_it{ m_models.begin() };
        m_it != m_models.end();
        ++m_it) {

        // follow path sequence if model is not currently colliding
        if (m_pathingEnabled) {
            std::vector<Model*>::iterator it{ std::find(
                collidingModels.begin(),
                collidingModels.end(),
                (*m_it)) };
            if (it == collidingModels.end())
                m_paths.at(modelIndex)->traverse((*m_it), deltaTime);
        }

        // play animation sequence
        if (m_animationsEnabled) {
            m_animations.at(modelIndex)->setSpeed(m_animationSpeedCurrent);
            m_animations.at(modelIndex)->play(*m_it, deltaTime);
        }

        // set shader attributes
        if (shader == m_shaderShadow)
            (*m_it)->setDepthShaderAttributes(shader);
        else if (shader == m_shaderEntity)
            (*m_it)->setColorShaderAttributes(shader);

        // render model hierarchy
        Model::ModelHierarchy* hierarchy = (*m_it)->getHierarchy();
        for (Model::ModelHierarchy::const_iterator e_it{ hierarchy->begin() };
            e_it != hierarchy->end();
            ++e_it) {

            // compute entity model matrix
            glm::mat4 modelMatrix;
            modelMatrix *= glm::scale(modelMatrix,
                    e_it->first->getScalingRelative())
                * getWorldOrientation()
                * (*m_it)->getModelMatrix(e_it->first)
                * e_it->first->getScalingMatrix();

            // set shader uniforms
            Shader::useProgram(shader->getProgramID());
            shader->setUniformMat4(UNIFORM_MATRIX_MODEL,
                modelMatrix);

            // render entity
            e_it->first->render(m_primitive);
        }

        // update path index
        ++modelIndex;
    }

    // go back to culling back faces
    glCullFace(GL_BACK);
}

void Renderer::renderParticles(GLfloat deltaTime,
    const glm::vec3& origin) {
    GLuint particleCount = static_cast<GLuint>(deltaTime * 100.0f);

    for (GLuint i{ 0 }; i != particleCount; ++i) {
        GLuint particle = findParticle();
        m_particles[particle].m_life = 5.0f;
        m_particles[particle].m_position = glm::vec3(
            static_cast<GLfloat>(rand() % GRID_SIZE - POSITION_MAX),
            LIGHT_POSITION_NOON.y,
            static_cast<GLfloat>(rand() % GRID_SIZE - POSITION_MAX));
    }

    glm::vec3 particleData[PARTICLE_COUNT];
    particleCount = 0;
    for (GLuint i{ 0 }; i != PARTICLE_COUNT; ++i) {
        Particle* p = &m_particles[i];

        if (p->m_life > 0.0f) {
            p->m_life -= deltaTime;

            if (p->m_life > 0.0f) {
                p->m_velocity += glm::vec3(glm::cos(deltaTime),
                    -9.81f,
                    glm::cos(deltaTime))
                    * 1000.0f
                    * deltaTime;
                p->m_position = p->m_velocity * deltaTime;
                p->m_distanceToCamera = glm::length(p->m_position
                    - Camera::get().getPosition());

                particleData[particleCount].x = p->m_position.x;
                particleData[particleCount].y = p->m_position.y;
                particleData[particleCount].z = p->m_position.z;
            }

            else
                p->m_distanceToCamera = -1.0f;

            ++particleCount;
        }
    }

    sortParticles();

    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBOPos);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(glm::vec3) * PARTICLE_COUNT,
        particleData,
        GL_STREAM_DRAW);
    /*glBufferSubData(GL_ARRAY_BUFFER,
        0,
        sizeof(glm::vec3) * 3 * particleCount,
        particleData);*/

    Shader::useProgram(m_shaderBlade->getProgramID());
    m_materials.at(3)->use(m_shaderBlade);
    m_shaderBlade->setUniformVec3(UNIFORM_CAMERA_RIGHT,
        Camera::get().getRightVector());
    m_shaderBlade->setUniformVec3(UNIFORM_CAMERA_UP,
        Camera::get().getUpVector());
    m_shaderBlade->setUniformMat4(UNIFORM_MATRIX_MODEL,
        Camera::get().getWorldOrientation());
    m_shaderBlade->setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderBlade->setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());

    glBindVertexArray(m_particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_particleEBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(GLfloat),
        (void*)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, m_particleVBOPos);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 1);

    glDrawArraysInstanced(GL_TRIANGLES,
        0,
        6,
        particleCount);
}

const glm::mat4& Renderer::getWorldOrientation() const {
    // return world orientation
    return Camera::get().getWorldOrientation();
}

glm::vec3 Renderer::getWorldAxis(const glm::vec3& axis) const {
    // return axis affected by world orientation
    return glm::vec3(getWorldOrientation() * glm::vec4(axis, 1.0f));
}

void Renderer::clampModelPosition(GLuint model) {
    // clamp model position
    if (m_modelPositions.at(model).x > POSITION_MAX)
        m_modelPositions.at(model).x = POSITION_MAX;
    else if (m_modelPositions.at(model).z > POSITION_MAX)
        m_modelPositions.at(model).z = POSITION_MAX;

    if (m_modelPositions.at(model).x < POSITION_MIN)
        m_modelPositions.at(model).x = POSITION_MIN;
    else if (m_modelPositions.at(model).z < POSITION_MIN)
        m_modelPositions.at(model).z = POSITION_MIN;
}

void Renderer::clampModelScale(GLuint model) {
    // clamp model scale
    if (m_modelScales.at(model).x > TRANSFORMATION_SCALE_MAX)
        m_modelScales.at(model) = glm::vec3(TRANSFORMATION_SCALE_MAX);
    else if (m_modelScales.at(model).x < TRANSFORMATION_SCALE_MIN)
        m_modelScales.at(model) = glm::vec3(TRANSFORMATION_SCALE_MIN);
}

glm::vec4 Renderer::lerpColor(const glm::vec4& start,
    const glm::vec4& end,
    GLfloat step) {
    // linearly interpolate between two colors
    return start + step * (end - start);
}

bool Renderer::isDawnOrDusk() const {
    // dawn or dusk
    return !(isDay() || isNight());
}

bool Renderer::isDay() const {
    // day time
    return (m_currentTime >= glm::pi<GLfloat>() / 6.0f
        && m_currentTime < glm::pi<GLfloat>() * 5.0f / 6.0f);
}

bool Renderer::isNight() const {
    // night time
    return (m_currentTime >= glm::pi<GLfloat>()
        && m_currentTime < 2.0f * glm::pi<GLfloat>());
}

GLuint Renderer::findParticle() {
    for (GLuint i{ m_lastParticle }; i != PARTICLE_COUNT; ++i) {
        if (m_particles[i].m_life < 0.0f) {
            m_lastParticle = i;
            return i;
        }
    }

    for (GLuint i{ 0 }; i != m_lastParticle; ++i) {
        if (m_particles[i].m_life < 0) {
            m_lastParticle = i;
            return i;
        }
    }

    return 0;
}

void Renderer::sortParticles() {
    std::sort(&m_particles[0], &m_particles[PARTICLE_COUNT - 1]);
}