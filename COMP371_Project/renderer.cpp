#include "renderer.h"

Renderer::~Renderer() {
    // free resources
    for (std::vector<Material>::const_iterator it{ m_materials.begin() };
        it != m_materials.end();
        ++it)
        it->free();
    m_shadowMap.free();

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
        m_modelPositions[model].x = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
        m_modelPositions[model].z = static_cast<GLfloat>(rand()
            % GRID_SIZE - POSITION_MAX);
        break;
    case Transform::Displacement::UP:
        m_modelPositions[model].x += TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::DOWN:
        m_modelPositions[model].x -= TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::LEFT:
        m_modelPositions[model].z -= TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    case Transform::Displacement::RIGHT:
        m_modelPositions[model].z += TRANSFORMATION_INCREMENT_TRANSLATION;
        break;
    }

    // clamp position between min and max values (the grid)
    clampModelPosition(model);

    // pass position value to model
    m_models.at(model).setPosition(m_modelPositions[model]);
}

void Renderer::resetModel(GLuint model) {
    // reset model
    m_modelPositions[model] = MODEL_POSITION_RELATIVE_TORSO;
    m_modelScales[model] = glm::vec3(1.0f, 1.0f, 1.0f);
    m_models.at(model).reset();
}

void Renderer::rotateModel(GLuint model,
    Transform::Displacement direction) {
    // rotate model
    switch (direction) {
    case Transform::Displacement::UP:
        m_models.at(model).rotate(TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Z);
        break;
    case Transform::Displacement::DOWN:
        m_models.at(model).rotate(-TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Z);
        break;
    case Transform::Displacement::LEFT:
        m_models.at(model).rotate(TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Y);
        break;
    case Transform::Displacement::RIGHT:
        m_models.at(model).rotate(-TRANSFORMATION_INCREMENT_ROTATION,
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
        m_models.at(model).rotateJoint(id,
            TRANSFORMATION_INCREMENT_ROTATION,
            AXIS_Z);
        break;
    case Transform::Displacement::DOWN:
        m_models.at(model).rotateJoint(id,
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
        m_modelScales[model] += TRANSFORMATION_INCREMENT_SCALING;
        break;
    case Transform::Scale::DECREASE:
        m_modelScales[model] -= TRANSFORMATION_INCREMENT_SCALING;
        break;
    }

    // clamp scale between min and max values
    clampModelScale(model);

    // pass scale value to model
    m_models.at(model).scale(m_modelScales[model]);
}

void Renderer::render(GLfloat deltaTime) {
    renderFirstPass(deltaTime);
    renderSecondPass(deltaTime);

    // optionally render shadow map debug quad
    if (m_debuggingEnabled)
        m_shadowMap.render();
}

void Renderer::toggleAnimations() {
    // set whether animations should be enabled or not
    m_animationsEnabled = !m_animationsEnabled;
    std::cout << "Animations: "
        << (m_animationsEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleDebugging() {
    // set whether framebuffer output should be enabled or not
    m_debuggingEnabled = !m_debuggingEnabled;
    std::cout << "Framebuffer output: "
        << (m_debuggingEnabled ? "ENABLED" : "DISABLED") << std::endl;
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

void Renderer::toggleShadows() {
    // set whether shadows should be enabled or not
    m_shadowsEnabled = !m_shadowsEnabled;
    std::cout << "Shadows: "
        << (m_shadowsEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::toggleTextures() {
    // set whether textures should be enabled or not
    m_texturesEnabled = !m_texturesEnabled;
    Material::toggleTextures();
    std::cout << "Textures: "
        << (m_texturesEnabled ? "ENABLED" : "DISABLED") << std::endl;
}

void Renderer::initialize() {
    // initialize random seed
    srand(static_cast<GLuint>(time(NULL)));

    // initialize model positions and scales
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i) {
        m_modelPositions[i] = MODEL_POSITION_RELATIVE_TORSO;
        m_modelScales[i] = glm::vec3(1.0f, 1.0f, 1.0f);
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
    initializeMaterial();
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i)
        initializeModel();
    initializePaths();
    initializeAnimation();
    initializeLight();
}

void Renderer::initializeAnimation() {
    // create animation
    Animation animation;

    // animation step 0
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        0,
        2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        0,
        1.0f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        0,
        10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        0,
        -15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        0,
        7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        0,
        -10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        0,
        10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        0,
        -15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        0,
        7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        0,
        -10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        0,
        1.0f));

    // animation step 1
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        1,
        2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        1,
        1.0f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        1,
        10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        1,
        -15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        1,
        7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        1,
        -10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        1,
        10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        1,
        -15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        1,
        7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        1,
        -10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        1,
        1.0f));

    // animation step 2
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        2,
        -2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        2,
        -1.0f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        2,
        -10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        2,
        15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        2,
        -7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        2,
        10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        2,
        -10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        2,
        15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        2,
        -7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        2,
        10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        2,
        -1.0f));

    // animation step 3
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        3,
        -2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        3,
        0.0f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        3,
        -10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        3,
        15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        3,
        -7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        3,
        10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        3,
        -10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        3,
        15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        3,
        -7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        3,
        10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        3,
        -1.0f));

    // animation step 4
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        4,
        -2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        4,
        0.0f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        4,
        -10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        4,
        15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        4,
        -7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        4,
        10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        4,
        -10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        4,
        -15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        4,
        -7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        4,
        10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        4,
        -1.0f));

    // animation step 5
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        5,
        2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        5,
        0.5f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        5,
        10.0f));
    animation.addStep(AnimationStep(3,
        AXIS_Z,
        5,
        -15.0f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        5,
        7.0f));
    animation.addStep(AnimationStep(5,
        AXIS_Z,
        5,
        -10.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        5,
        10.0f));
    animation.addStep(AnimationStep(7,
        AXIS_Z,
        5,
        -15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        5,
        7.0f));
    animation.addStep(AnimationStep(9,
        AXIS_Z,
        5,
        -10.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        5,
        1.0f));

    // animation step 6
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        6,
        -2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        6,
        0.5f));
    animation.addStep(AnimationStep(2,
        AXIS_Z,
        6,
        -15.0f));
    animation.addStep(AnimationStep(8,
        AXIS_Z,
        6,
        -15.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        6,
        1.0f));

    // animation step 7
    animation.addStep(AnimationStep(0,
        AXIS_Z,
        7,
        -2.0f));
    animation.addStep(AnimationStep(1,
        AXIS_Z,
        7,
        -0.5f));
    animation.addStep(AnimationStep(4,
        AXIS_Z,
        7,
        -15.0f));
    animation.addStep(AnimationStep(6,
        AXIS_Z,
        7,
        -15.0f));
    animation.addStep(AnimationStep(10,
        AXIS_Z,
        7,
        -1.0f));

    // add animation to animations vector
    m_animations.emplace_back(std::move(animation));
}

void Renderer::initializeFrame() {
    // axes vertex data
    GLfloat verticesAxes[] = {
        // position
        0.0f, 0.0f, 0.0f,   // line origin
        5.0f, 0.0f, 0.0f    // line end
    };

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_axesVAO);
    glBindVertexArray(m_axesVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_axesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_axesVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(verticesAxes),
        verticesAxes,
        GL_STATIC_DRAW);

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shaderFrame.getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);

    // grid vertex data
    GLfloat verticesGrid[] = {
        // horizontal lines
        -50.0f, 0.0f, -50.0f,
         50.0f, 0.0f, -50.0f,
        -50.0f, 0.0f, -49.0f,
         50.0f, 0.0f, -49.0f,
        -50.0f, 0.0f, -48.0f,
         50.0f, 0.0f, -48.0f,
        -50.0f, 0.0f, -47.0f,
         50.0f, 0.0f, -47.0f,
        -50.0f, 0.0f, -46.0f,
         50.0f, 0.0f, -46.0f,
        -50.0f, 0.0f, -45.0f,
         50.0f, 0.0f, -45.0f,
        -50.0f, 0.0f, -44.0f,
         50.0f, 0.0f, -44.0f,
        -50.0f, 0.0f, -43.0f,
         50.0f, 0.0f, -43.0f,
        -50.0f, 0.0f, -42.0f,
         50.0f, 0.0f, -42.0f,
        -50.0f, 0.0f, -41.0f,
         50.0f, 0.0f, -41.0f,
        -50.0f, 0.0f, -40.0f,
         50.0f, 0.0f, -40.0f,
        -50.0f, 0.0f, -39.0f,
         50.0f, 0.0f, -39.0f,
        -50.0f, 0.0f, -38.0f,
         50.0f, 0.0f, -38.0f,
        -50.0f, 0.0f, -37.0f,
         50.0f, 0.0f, -37.0f,
        -50.0f, 0.0f, -36.0f,
         50.0f, 0.0f, -36.0f,
        -50.0f, 0.0f, -35.0f,
         50.0f, 0.0f, -35.0f,
        -50.0f, 0.0f, -34.0f,
         50.0f, 0.0f, -34.0f,
        -50.0f, 0.0f, -33.0f,
         50.0f, 0.0f, -33.0f,
        -50.0f, 0.0f, -32.0f,
         50.0f, 0.0f, -32.0f,
        -50.0f, 0.0f, -31.0f,
         50.0f, 0.0f, -31.0f,
        -50.0f, 0.0f, -30.0f,
         50.0f, 0.0f, -30.0f,
        -50.0f, 0.0f, -29.0f,
         50.0f, 0.0f, -29.0f,
        -50.0f, 0.0f, -28.0f,
         50.0f, 0.0f, -28.0f,
        -50.0f, 0.0f, -27.0f,
         50.0f, 0.0f, -27.0f,
        -50.0f, 0.0f, -26.0f,
         50.0f, 0.0f, -26.0f,
        -50.0f, 0.0f, -25.0f,
         50.0f, 0.0f, -25.0f,
        -50.0f, 0.0f, -24.0f,
         50.0f, 0.0f, -24.0f,
        -50.0f, 0.0f, -23.0f,
         50.0f, 0.0f, -23.0f,
        -50.0f, 0.0f, -22.0f,
         50.0f, 0.0f, -22.0f,
        -50.0f, 0.0f, -21.0f,
         50.0f, 0.0f, -21.0f,
        -50.0f, 0.0f, -20.0f,
         50.0f, 0.0f, -20.0f,
        -50.0f, 0.0f, -19.0f,
         50.0f, 0.0f, -19.0f,
        -50.0f, 0.0f, -18.0f,
         50.0f, 0.0f, -18.0f,
        -50.0f, 0.0f, -17.0f,
         50.0f, 0.0f, -17.0f,
        -50.0f, 0.0f, -16.0f,
         50.0f, 0.0f, -16.0f,
        -50.0f, 0.0f, -15.0f,
         50.0f, 0.0f, -15.0f,
        -50.0f, 0.0f, -14.0f,
         50.0f, 0.0f, -14.0f,
        -50.0f, 0.0f, -13.0f,
         50.0f, 0.0f, -13.0f,
        -50.0f, 0.0f, -12.0f,
         50.0f, 0.0f, -12.0f,
        -50.0f, 0.0f, -11.0f,
         50.0f, 0.0f, -11.0f,
        -50.0f, 0.0f, -10.0f,
         50.0f, 0.0f, -10.0f,
        -50.0f, 0.0f, -9.0f,
         50.0f, 0.0f, -9.0f,
        -50.0f, 0.0f, -8.0f,
         50.0f, 0.0f, -8.0f,
        -50.0f, 0.0f, -7.0f,
         50.0f, 0.0f, -7.0f,
        -50.0f, 0.0f, -6.0f,
         50.0f, 0.0f, -6.0f,
        -50.0f, 0.0f, -5.0f,
         50.0f, 0.0f, -5.0f,
        -50.0f, 0.0f, -4.0f,
         50.0f, 0.0f, -4.0f,
        -50.0f, 0.0f, -3.0f,
         50.0f, 0.0f, -3.0f,
        -50.0f, 0.0f, -2.0f,
         50.0f, 0.0f, -2.0f,
        -50.0f, 0.0f, -1.0f,
         50.0f, 0.0f, -1.0f,
        -50.0f, 0.0f,  0.0f,
         50.0f, 0.0f,  0.0f,
        -50.0f, 0.0f,  1.0f,
         50.0f, 0.0f,  1.0f,
        -50.0f, 0.0f,  2.0f,
         50.0f, 0.0f,  2.0f,
        -50.0f, 0.0f,  3.0f,
         50.0f, 0.0f,  3.0f,
        -50.0f, 0.0f,  4.0f,
         50.0f, 0.0f,  4.0f,
        -50.0f, 0.0f,  5.0f,
         50.0f, 0.0f,  5.0f,
        -50.0f, 0.0f,  6.0f,
         50.0f, 0.0f,  6.0f,
        -50.0f, 0.0f,  7.0f,
         50.0f, 0.0f,  7.0f,
        -50.0f, 0.0f,  8.0f,
         50.0f, 0.0f,  8.0f,
        -50.0f, 0.0f,  9.0f,
         50.0f, 0.0f,  9.0f,
        -50.0f, 0.0f, 10.0f,
         50.0f, 0.0f, 10.0f,
        -50.0f, 0.0f, 11.0f,
         50.0f, 0.0f, 11.0f,
        -50.0f, 0.0f, 12.0f,
         50.0f, 0.0f, 12.0f,
        -50.0f, 0.0f, 13.0f,
         50.0f, 0.0f, 13.0f,
        -50.0f, 0.0f, 14.0f,
         50.0f, 0.0f, 14.0f,
        -50.0f, 0.0f, 15.0f,
         50.0f, 0.0f, 15.0f,
        -50.0f, 0.0f, 16.0f,
         50.0f, 0.0f, 16.0f,
        -50.0f, 0.0f, 17.0f,
         50.0f, 0.0f, 17.0f,
        -50.0f, 0.0f, 18.0f,
         50.0f, 0.0f, 18.0f,
        -50.0f, 0.0f, 19.0f,
         50.0f, 0.0f, 19.0f,
        -50.0f, 0.0f, 20.0f,
         50.0f, 0.0f, 20.0f,
        -50.0f, 0.0f, 21.0f,
         50.0f, 0.0f, 21.0f,
        -50.0f, 0.0f, 22.0f,
         50.0f, 0.0f, 22.0f,
        -50.0f, 0.0f, 23.0f,
         50.0f, 0.0f, 23.0f,
        -50.0f, 0.0f, 24.0f,
         50.0f, 0.0f, 24.0f,
        -50.0f, 0.0f, 25.0f,
         50.0f, 0.0f, 25.0f,
        -50.0f, 0.0f, 26.0f,
         50.0f, 0.0f, 26.0f,
        -50.0f, 0.0f, 27.0f,
         50.0f, 0.0f, 27.0f,
        -50.0f, 0.0f, 28.0f,
         50.0f, 0.0f, 28.0f,
        -50.0f, 0.0f, 29.0f,
         50.0f, 0.0f, 29.0f,
        -50.0f, 0.0f, 30.0f,
         50.0f, 0.0f, 30.0f,
        -50.0f, 0.0f, 31.0f,
         50.0f, 0.0f, 31.0f,
        -50.0f, 0.0f, 32.0f,
         50.0f, 0.0f, 32.0f,
        -50.0f, 0.0f, 33.0f,
         50.0f, 0.0f, 33.0f,
        -50.0f, 0.0f, 34.0f,
         50.0f, 0.0f, 34.0f,
        -50.0f, 0.0f, 35.0f,
         50.0f, 0.0f, 35.0f,
        -50.0f, 0.0f, 36.0f,
         50.0f, 0.0f, 36.0f,
        -50.0f, 0.0f, 37.0f,
         50.0f, 0.0f, 37.0f,
        -50.0f, 0.0f, 38.0f,
         50.0f, 0.0f, 38.0f,
        -50.0f, 0.0f, 39.0f,
         50.0f, 0.0f, 39.0f,
        -50.0f, 0.0f, 40.0f,
         50.0f, 0.0f, 40.0f,
        -50.0f, 0.0f, 41.0f,
         50.0f, 0.0f, 41.0f,
        -50.0f, 0.0f, 42.0f,
         50.0f, 0.0f, 42.0f,
        -50.0f, 0.0f, 43.0f,
         50.0f, 0.0f, 43.0f,
        -50.0f, 0.0f, 44.0f,
         50.0f, 0.0f, 44.0f,
        -50.0f, 0.0f, 45.0f,
         50.0f, 0.0f, 45.0f,
        -50.0f, 0.0f, 46.0f,
         50.0f, 0.0f, 46.0f,
        -50.0f, 0.0f, 47.0f,
         50.0f, 0.0f, 47.0f,
        -50.0f, 0.0f, 48.0f,
         50.0f, 0.0f, 48.0f,
        -50.0f, 0.0f, 49.0f,
         50.0f, 0.0f, 49.0f,
        -50.0f, 0.0f, 50.0f,
         50.0f, 0.0f, 50.0f,

        // vertical lines
        -50.0f, 0.0f, -50.0f,
        -50.0f, 0.0f,  50.0f,
        -49.0f, 0.0f, -50.0f,
        -49.0f, 0.0f,  50.0f,
        -48.0f, 0.0f, -50.0f,
        -48.0f, 0.0f,  50.0f,
        -47.0f, 0.0f, -50.0f,
        -47.0f, 0.0f,  50.0f,
        -46.0f, 0.0f, -50.0f,
        -46.0f, 0.0f,  50.0f,
        -45.0f, 0.0f, -50.0f,
        -45.0f, 0.0f,  50.0f,
        -44.0f, 0.0f, -50.0f,
        -44.0f, 0.0f,  50.0f,
        -43.0f, 0.0f, -50.0f,
        -43.0f, 0.0f,  50.0f,
        -42.0f, 0.0f, -50.0f,
        -42.0f, 0.0f,  50.0f,
        -41.0f, 0.0f, -50.0f,
        -41.0f, 0.0f,  50.0f,
        -40.0f, 0.0f, -50.0f,
        -40.0f, 0.0f,  50.0f,
        -39.0f, 0.0f, -50.0f,
        -39.0f, 0.0f,  50.0f,
        -38.0f, 0.0f, -50.0f,
        -38.0f, 0.0f,  50.0f,
        -37.0f, 0.0f, -50.0f,
        -37.0f, 0.0f,  50.0f,
        -36.0f, 0.0f, -50.0f,
        -36.0f, 0.0f,  50.0f,
        -35.0f, 0.0f, -50.0f,
        -35.0f, 0.0f,  50.0f,
        -34.0f, 0.0f, -50.0f,
        -34.0f, 0.0f,  50.0f,
        -33.0f, 0.0f, -50.0f,
        -33.0f, 0.0f,  50.0f,
        -32.0f, 0.0f, -50.0f,
        -32.0f, 0.0f,  50.0f,
        -31.0f, 0.0f, -50.0f,
        -31.0f, 0.0f,  50.0f,
        -30.0f, 0.0f, -50.0f,
        -30.0f, 0.0f,  50.0f,
        -29.0f, 0.0f, -50.0f,
        -29.0f, 0.0f,  50.0f,
        -28.0f, 0.0f, -50.0f,
        -28.0f, 0.0f,  50.0f,
        -27.0f, 0.0f, -50.0f,
        -27.0f, 0.0f,  50.0f,
        -26.0f, 0.0f, -50.0f,
        -26.0f, 0.0f,  50.0f,
        -25.0f, 0.0f, -50.0f,
        -25.0f, 0.0f,  50.0f,
        -24.0f, 0.0f, -50.0f,
        -24.0f, 0.0f,  50.0f,
        -23.0f, 0.0f, -50.0f,
        -23.0f, 0.0f,  50.0f,
        -22.0f, 0.0f, -50.0f,
        -22.0f, 0.0f,  50.0f,
        -21.0f, 0.0f, -50.0f,
        -21.0f, 0.0f,  50.0f,
        -20.0f, 0.0f, -50.0f,
        -20.0f, 0.0f,  50.0f,
        -19.0f, 0.0f, -50.0f,
        -19.0f, 0.0f,  50.0f,
        -18.0f, 0.0f, -50.0f,
        -18.0f, 0.0f,  50.0f,
        -17.0f, 0.0f, -50.0f,
        -17.0f, 0.0f,  50.0f,
        -16.0f, 0.0f, -50.0f,
        -16.0f, 0.0f,  50.0f,
        -15.0f, 0.0f, -50.0f,
        -15.0f, 0.0f,  50.0f,
        -14.0f, 0.0f, -50.0f,
        -14.0f, 0.0f,  50.0f,
        -13.0f, 0.0f, -50.0f,
        -13.0f, 0.0f,  50.0f,
        -12.0f, 0.0f, -50.0f,
        -12.0f, 0.0f,  50.0f,
        -11.0f, 0.0f, -50.0f,
        -11.0f, 0.0f,  50.0f,
        -10.0f, 0.0f, -50.0f,
        -10.0f, 0.0f,  50.0f,
         -9.0f, 0.0f, -50.0f,
         -9.0f, 0.0f,  50.0f,
         -8.0f, 0.0f, -50.0f,
         -8.0f, 0.0f,  50.0f,
         -7.0f, 0.0f, -50.0f,
         -7.0f, 0.0f,  50.0f,
         -6.0f, 0.0f, -50.0f,
         -6.0f, 0.0f,  50.0f,
         -5.0f, 0.0f, -50.0f,
         -5.0f, 0.0f,  50.0f,
         -4.0f, 0.0f, -50.0f,
         -4.0f, 0.0f,  50.0f,
         -3.0f, 0.0f, -50.0f,
         -3.0f, 0.0f,  50.0f,
         -2.0f, 0.0f, -50.0f,
         -2.0f, 0.0f,  50.0f,
         -1.0f, 0.0f, -50.0f,
         -1.0f, 0.0f,  50.0f,
          0.0f, 0.0f, -50.0f,
          0.0f, 0.0f,  50.0f,
          1.0f, 0.0f, -50.0f,
          1.0f, 0.0f,  50.0f,
          2.0f, 0.0f, -50.0f,
          2.0f, 0.0f,  50.0f,
          3.0f, 0.0f, -50.0f,
          3.0f, 0.0f,  50.0f,
          4.0f, 0.0f, -50.0f,
          4.0f, 0.0f,  50.0f,
          5.0f, 0.0f, -50.0f,
          5.0f, 0.0f,  50.0f,
          6.0f, 0.0f, -50.0f,
          6.0f, 0.0f,  50.0f,
          7.0f, 0.0f, -50.0f,
          7.0f, 0.0f,  50.0f,
          8.0f, 0.0f, -50.0f,
          8.0f, 0.0f,  50.0f,
          9.0f, 0.0f, -50.0f,
          9.0f, 0.0f,  50.0f,
         10.0f, 0.0f, -50.0f,
         10.0f, 0.0f,  50.0f,
         11.0f, 0.0f, -50.0f,
         11.0f, 0.0f,  50.0f,
         12.0f, 0.0f, -50.0f,
         12.0f, 0.0f,  50.0f,
         13.0f, 0.0f, -50.0f,
         13.0f, 0.0f,  50.0f,
         14.0f, 0.0f, -50.0f,
         14.0f, 0.0f,  50.0f,
         15.0f, 0.0f, -50.0f,
         15.0f, 0.0f,  50.0f,
         16.0f, 0.0f, -50.0f,
         16.0f, 0.0f,  50.0f,
         17.0f, 0.0f, -50.0f,
         17.0f, 0.0f,  50.0f,
         18.0f, 0.0f, -50.0f,
         18.0f, 0.0f,  50.0f,
         19.0f, 0.0f, -50.0f,
         19.0f, 0.0f,  50.0f,
         20.0f, 0.0f, -50.0f,
         20.0f, 0.0f,  50.0f,
         21.0f, 0.0f, -50.0f,
         21.0f, 0.0f,  50.0f,
         22.0f, 0.0f, -50.0f,
         22.0f, 0.0f,  50.0f,
         23.0f, 0.0f, -50.0f,
         23.0f, 0.0f,  50.0f,
         24.0f, 0.0f, -50.0f,
         24.0f, 0.0f,  50.0f,
         25.0f, 0.0f, -50.0f,
         25.0f, 0.0f,  50.0f,
         26.0f, 0.0f, -50.0f,
         26.0f, 0.0f,  50.0f,
         27.0f, 0.0f, -50.0f,
         27.0f, 0.0f,  50.0f,
         28.0f, 0.0f, -50.0f,
         28.0f, 0.0f,  50.0f,
         29.0f, 0.0f, -50.0f,
         29.0f, 0.0f,  50.0f,
         30.0f, 0.0f, -50.0f,
         30.0f, 0.0f,  50.0f,
         31.0f, 0.0f, -50.0f,
         31.0f, 0.0f,  50.0f,
         32.0f, 0.0f, -50.0f,
         32.0f, 0.0f,  50.0f,
         33.0f, 0.0f, -50.0f,
         33.0f, 0.0f,  50.0f,
         34.0f, 0.0f, -50.0f,
         34.0f, 0.0f,  50.0f,
         35.0f, 0.0f, -50.0f,
         35.0f, 0.0f,  50.0f,
         36.0f, 0.0f, -50.0f,
         36.0f, 0.0f,  50.0f,
         37.0f, 0.0f, -50.0f,
         37.0f, 0.0f,  50.0f,
         38.0f, 0.0f, -50.0f,
         38.0f, 0.0f,  50.0f,
         39.0f, 0.0f, -50.0f,
         39.0f, 0.0f,  50.0f,
         40.0f, 0.0f, -50.0f,
         40.0f, 0.0f,  50.0f,
         41.0f, 0.0f, -50.0f,
         41.0f, 0.0f,  50.0f,
         42.0f, 0.0f, -50.0f,
         42.0f, 0.0f,  50.0f,
         43.0f, 0.0f, -50.0f,
         43.0f, 0.0f,  50.0f,
         44.0f, 0.0f, -50.0f,
         44.0f, 0.0f,  50.0f,
         45.0f, 0.0f, -50.0f,
         45.0f, 0.0f,  50.0f,
         46.0f, 0.0f, -50.0f,
         46.0f, 0.0f,  50.0f,
         47.0f, 0.0f, -50.0f,
         47.0f, 0.0f,  50.0f,
         48.0f, 0.0f, -50.0f,
         48.0f, 0.0f,  50.0f,
         49.0f, 0.0f, -50.0f,
         49.0f, 0.0f,  50.0f,
         50.0f, 0.0f, -50.0f,
         50.0f, 0.0f,  50.0f
    };

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_gridVAO);
    glBindVertexArray(m_gridVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_gridVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(verticesGrid),
        verticesGrid,
        GL_STATIC_DRAW);

    // vertex attributes
    positionLocation = glGetAttribLocation(m_shaderFrame.getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);
}

void Renderer::initializeGround() {
    // vertex data
    GLfloat verticesGround[] = {
        // position             // normal           // texture
        -50.0f, 0.0f, -50.0f,   0.0f, 1.0f, 0.0f,    0.0f, 64.0f,
        -50.0f, 0.0f,  50.0f,   0.0f, 1.0f, 0.0f,    0.0f,  0.0f,
         50.0f, 0.0f,  50.0f,   0.0f, 1.0f, 0.0f,   64.0f,  0.0f,
         50.0f, 0.0f, -50.0f,   0.0f, 1.0f, 0.0f,   64.0f, 64.0f
    };

    GLuint indicesGround[] = {
        0, 1, 2,
        0, 2, 3
    };

    // initialize ground material
    m_materials.emplace_back(Texture(PATH_TEXTURE_GROUND,
        GL_RGBA,
        GL_REPEAT,
        GL_LINEAR).getID(),
        MATERIAL_SHININESS_GROUND);

    // add ground entity to entities vector
    m_entities.emplace_back(&m_shaderEntity,
        POSITION_ORIGIN,
        verticesGround,
        sizeof(verticesGround),
        indicesGround,
        sizeof(indicesGround));

    // set ground color
    m_entities.at(0).setColor(COLOR_GROUND);
}

void Renderer::initializeLight() {
    // vertex data
    GLfloat vertices[] = {
        // back face
        // position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // front face
        // position
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // left face
        // position
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // right face
        // position
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        // bottom face
        // position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        // top face
        // position
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    // generate and bind vertex array object
    glGenVertexArrays(1, &m_lightVAO);
    glBindVertexArray(m_lightVAO);

    // generate and bind vertex buffer object, buffer data
    glGenBuffers(1, &m_lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lightVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW);

    // vertex attributes
    GLuint positionLocation = glGetAttribLocation(m_shaderFrame.getProgramID(),
        ATTRIBUTE_POSITION.c_str());
    glVertexAttribPointer(positionLocation,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0);
    glEnableVertexAttribArray(positionLocation);

    // create light source and add it to lights vector
    m_lights.emplace_back(LIGHT_POSITION);
}

void Renderer::initializeMaterial() {
    // initialize model material
    m_materials.emplace_back(Texture(PATH_TEXTURE_HORSE,
        GL_RGB,
        GL_CLAMP_TO_EDGE,
        GL_LINEAR).getID(),
        MATERIAL_SHININESS_HORSE);
}

void Renderer::initializeModel() {
    // vertex data
    GLfloat vertices[] = {
        // back face
        // position           // normal             // texture
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        // front face
        // position           // normal             // texture
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        // left face
        // position           // normal             // texture
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

        // right face
        // position           // normal             // texture
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f,

        // bottom face
        // position           // normal             // texture
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f,

        // top face
        // position           // normal             // texture
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };

    // create model entities
    RenderedEntity head(&m_shaderEntity,
        POSITION_ORIGIN,
        vertices,
        sizeof(vertices));
    RenderedEntity legLowerFrontRight(head);
    RenderedEntity legLowerFrontLeft(head);
    RenderedEntity legLowerBackRight(head);
    RenderedEntity legLowerBackLeft(head);
    RenderedEntity legUpperFrontRight(head);
    RenderedEntity legUpperFrontLeft(head);
    RenderedEntity legUpperBackRight(head);
    RenderedEntity legUpperBackLeft(head);
    RenderedEntity neck(head);
    RenderedEntity torso(head);

    // set body parts color
    head.setColor(MODEL_COLOR_HEAD);
    legLowerFrontRight.setColor(MODEL_COLOR_LEG_LOWER);
    legLowerFrontLeft.setColor(MODEL_COLOR_LEG_LOWER);
    legLowerBackRight.setColor(MODEL_COLOR_LEG_LOWER);
    legLowerBackLeft.setColor(MODEL_COLOR_LEG_LOWER);
    legUpperFrontRight.setColor(MODEL_COLOR_LEG_UPPER);
    legUpperFrontLeft.setColor(MODEL_COLOR_LEG_UPPER);
    legUpperBackRight.setColor(MODEL_COLOR_LEG_UPPER);
    legUpperBackLeft.setColor(MODEL_COLOR_LEG_UPPER);
    neck.setColor(MODEL_COLOR_NECK);
    torso.setColor(MODEL_COLOR_TORSO);

    // scale body parts
    head.scale(MODEL_SCALE_HEAD);
    legLowerFrontRight.scale(MODEL_SCALE_LEG_LOWER);
    legLowerFrontLeft.scale(MODEL_SCALE_LEG_LOWER);
    legLowerBackRight.scale(MODEL_SCALE_LEG_LOWER);
    legLowerBackLeft.scale(MODEL_SCALE_LEG_LOWER);
    legUpperFrontRight.scale(MODEL_SCALE_LEG_UPPER);
    legUpperFrontLeft.scale(MODEL_SCALE_LEG_UPPER);
    legUpperBackRight.scale(MODEL_SCALE_LEG_UPPER);
    legUpperBackLeft.scale(MODEL_SCALE_LEG_UPPER);
    neck.scale(MODEL_SCALE_NECK);
    torso.scale(MODEL_SCALE_TORSO);

    // translate body parts to their appropriate locations
    head.translate(MODEL_POSITION_RELATIVE_HEAD);
    legLowerFrontRight.translate(MODEL_POSITION_RELATIVE_LEG_LOWER_FR);
    legLowerFrontLeft.translate(MODEL_POSITION_RELATIVE_LEG_LOWER_FL);
    legLowerBackRight.translate(MODEL_POSITION_RELATIVE_LEG_LOWER_BR);
    legLowerBackLeft.translate(MODEL_POSITION_RELATIVE_LEG_LOWER_BL);
    legUpperFrontRight.translate(MODEL_POSITION_RELATIVE_LEG_UPPER_FR);
    legUpperFrontLeft.translate(MODEL_POSITION_RELATIVE_LEG_UPPER_FL);
    legUpperBackRight.translate(MODEL_POSITION_RELATIVE_LEG_UPPER_BR);
    legUpperBackLeft.translate(MODEL_POSITION_RELATIVE_LEG_UPPER_BL);
    neck.translate(MODEL_POSITION_RELATIVE_NECK);
    torso.translate(MODEL_POSITION_RELATIVE_TORSO);

    // rotate head and neck, set torso rotation
    head.rotate(MODEL_ORIENTATION_RELATIVE_HEAD.first,
        MODEL_ORIENTATION_RELATIVE_HEAD.second);
    neck.rotate(MODEL_ORIENTATION_RELATIVE_NECK.first,
        MODEL_ORIENTATION_RELATIVE_NECK.second);
    torso.setRotation(true);

    // add body parts to entities vector
    m_entities.emplace_back(std::move(head));
    m_entities.emplace_back(std::move(legLowerFrontRight));
    m_entities.emplace_back(std::move(legLowerFrontLeft));
    m_entities.emplace_back(std::move(legLowerBackRight));
    m_entities.emplace_back(std::move(legLowerBackLeft));
    m_entities.emplace_back(std::move(legUpperFrontRight));
    m_entities.emplace_back(std::move(legUpperFrontLeft));
    m_entities.emplace_back(std::move(legUpperBackRight));
    m_entities.emplace_back(std::move(legUpperBackLeft));
    m_entities.emplace_back(std::move(neck));
    m_entities.emplace_back(std::move(torso));

    // create model object
    m_models.emplace_back(Model());
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
    m_models.at(modelIndex).add(&m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesNeck),
        &m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesHead),
        &m_entities.at(entitiesNeck));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegUpperFrontRight),
        &m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegUpperFrontLeft),
        &m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegUpperBackRight),
        &m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegUpperBackLeft),
        &m_entities.at(entitiesTorso));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegLowerFrontRight),
        &m_entities.at(entitiesLegUpperFrontRight));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegLowerFrontLeft),
        &m_entities.at(entitiesLegUpperFrontLeft));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegLowerBackRight),
        &m_entities.at(entitiesLegUpperBackRight));
    m_models.at(modelIndex).attach(&m_entities.at(entitiesLegLowerBackLeft),
        &m_entities.at(entitiesLegUpperBackLeft));

    // create model joints
    Joint joint0(&m_entities.at(entitiesNeck),
        &m_entities.at(entitiesHead),
        0.5f * MODEL_POSITION_RELATIVE_HEAD,
        MODEL_ROTATION_HEAD_MAX,
        MODEL_ROTATION_HEAD_MIN);
    Joint joint1(&m_entities.at(entitiesTorso),
        &m_entities.at(entitiesNeck),
        0.5f * MODEL_POSITION_RELATIVE_NECK,
        MODEL_ROTATION_NECK_MAX,
        MODEL_ROTATION_NECK_MIN);
    Joint joint2(&m_entities.at(entitiesTorso),
        &m_entities.at(entitiesLegUpperFrontRight),
        0.5f * MODEL_SCALE_LEG_UPPER
        * MODEL_POSITION_RELATIVE_LEG_UPPER_FR,
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint joint3(&m_entities.at(entitiesLegUpperFrontRight),
        &m_entities.at(entitiesLegLowerFrontRight),
        0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_FR,
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint joint4(&m_entities.at(entitiesTorso),
        &m_entities.at(entitiesLegUpperBackRight),
        0.5f * MODEL_SCALE_LEG_UPPER
        * MODEL_POSITION_RELATIVE_LEG_UPPER_BR,
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint joint5(&m_entities.at(entitiesLegUpperBackRight),
        &m_entities.at(entitiesLegLowerBackRight),
        0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_BR,
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint joint6(&m_entities.at(entitiesTorso),
        &m_entities.at(entitiesLegUpperFrontLeft),
        0.5f * MODEL_SCALE_LEG_UPPER
        * MODEL_POSITION_RELATIVE_LEG_UPPER_FL,
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint joint7(&m_entities.at(entitiesLegUpperFrontLeft),
        &m_entities.at(entitiesLegLowerFrontLeft),
        0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_FL,
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint joint8(&m_entities.at(entitiesTorso),
        &m_entities.at(entitiesLegUpperBackLeft),
        0.5f * MODEL_SCALE_LEG_UPPER
        * MODEL_POSITION_RELATIVE_LEG_UPPER_BL,
        MODEL_ROTATION_LEG_UPPER_MAX,
        MODEL_ROTATION_LEG_UPPER_MIN);
    Joint joint9(&m_entities.at(entitiesLegUpperBackLeft),
        &m_entities.at(entitiesLegLowerBackLeft),
        0.5f * MODEL_POSITION_RELATIVE_LEG_LOWER_BL,
        MODEL_ROTATION_LEG_LOWER_MAX,
        MODEL_ROTATION_LEG_LOWER_MIN);
    Joint joint10(nullptr,
        &m_entities.at(entitiesTorso),
        POSITION_ORIGIN,
        MODEL_ROTATION_TORSO_MAX,
        MODEL_ROTATION_TORSO_MIN);

    // add model joints to joints vector
    m_joints.emplace_back(std::move(joint0));
    m_joints.emplace_back(std::move(joint1));
    m_joints.emplace_back(std::move(joint2));
    m_joints.emplace_back(std::move(joint3));
    m_joints.emplace_back(std::move(joint4));
    m_joints.emplace_back(std::move(joint5));
    m_joints.emplace_back(std::move(joint6));
    m_joints.emplace_back(std::move(joint7));
    m_joints.emplace_back(std::move(joint8));
    m_joints.emplace_back(std::move(joint9));
    m_joints.emplace_back(std::move(joint10));

    // add joints to model
    GLuint jointStart = modelIndex * 11;
    GLuint jointEnd = jointStart + 11;
    for (GLuint j{ jointStart }; j != jointEnd; ++j)
        m_models.at(modelIndex).addJoint(&m_joints.at(j));

    // place horse at random location
    if (modelIndex != 0)
        moveModel(modelIndex, Transform::RANDOM);
}

void Renderer::initializePaths() {
    for (GLuint i{ 0 }; i != TROOP_COUNT; ++i) {

    }
}

void Renderer::renderFirstPass(GLfloat deltaTime) {
    // compute depth texture transformation matrices
    glm::mat4 shadowProjection = glm::perspective(
        glm::radians(SHADOW_PROJECTION_FOV),
        SHADOW_ASPECT_RATIO,
        m_lights.at(0).getPlaneNear(),
        m_lights.at(0).getPlaneFar());
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTransforms.push_back(shadowProjection
        * glm::lookAt(m_lights.at(0).getWorldPosition(
                getWorldOrientation()),
            m_lights.at(0).getWorldPosition(
                getWorldOrientation())
            + glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, -1.0f, 0.0f)));

    // set viewport to depth texture dimensions
    glViewport(0,
        0,
        SHADOW_DEPTH_TEXTURE_WIDTH,
        SHADOW_DEPTH_TEXTURE_HEIGHT);

    // bind and clear shadow map framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowMap.getFBOID());
    glClear(GL_DEPTH_BUFFER_BIT);

    // set shadow map shader uniforms
    m_shaderShadow.use();
    for (GLuint i{ 0 }; i != shadowTransforms.size(); ++i)
        m_shaderShadow.setUniformMat4(UNIFORM_SHADOW_TRANSFORMS
            + "[" + std::to_string(i) + "]",
            shadowTransforms[i]);
    m_shaderShadow.setUniformVec3(UNIFORM_LIGHT_POSITION,
        m_lights.at(0).getWorldPosition(
            getWorldOrientation()));
    m_shaderShadow.setUniformVec2(UNIFORM_LIGHT_PLANES,
        glm::vec2(m_lights.at(0).getPlaneNear(),
            m_lights.at(0).getPlaneFar()));

    // render ground to depth texture
    renderGround(&m_shaderShadow);

    // render models to depth texture
    renderModels(&m_shaderShadow, deltaTime);

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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render skybox
    m_skybox.render(Camera::get().getWorldOrientation(),
        Camera::get().getViewMatrix(),
        Camera::get().getProjectionMatrix(),
        Camera::get().getPosition());

    // shader uniforms: transformations
    m_shaderEntity.use();
    m_shaderEntity.setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderEntity.setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());

    // shader uniforms: lighting
    m_shaderEntity.setUniformVec2(UNIFORM_LIGHT_PLANES,
        glm::vec2(m_lights.at(0).getPlaneNear(),
            m_lights.at(0).getPlaneFar()));
    m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_POSITION,
        m_lights.at(0).getWorldPosition(
            getWorldOrientation()));
    m_shaderEntity.setUniformVec3(UNIFORM_CAMERA_POSITION,
        Camera::get().getPosition());
    if (m_lightsEnabled) {
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            m_lights.at(0).getAmbient());
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            m_lights.at(0).getDiffuse());
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            m_lights.at(0).getSpecular());
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KC,
            m_lights.at(0).getKC());
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KL,
            m_lights.at(0).getKL());
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KQ,
            m_lights.at(0).getKQ());
    }
    else {
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_AMBIENT,
            glm::vec3(1.0f, 1.0f, 1.0f));
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_DIFFUSE,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderEntity.setUniformVec3(UNIFORM_LIGHT_SPECULAR,
            glm::vec3(0.0f, 0.0f, 0.0f));
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KC,
            1.0f);
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KL,
            0.0f);
        m_shaderEntity.setUniformFloat(UNIFORM_LIGHT_KQ,
            0.0f);
    }

    // shader uniforms: shadows
    m_shaderEntity.setUniformBool(UNIFORM_SHADOWS_ENABLED,
        m_shadowsEnabled);
    m_shaderEntity.setUniformBool(UNIFORM_TEXTURES_ENABLED,
        m_texturesEnabled);
    m_shaderEntity.setUniformUInt(UNIFORM_SHADOW_GRID_SAMPLES,
        SHADOW_GRID_SAMPLES);
    m_shaderEntity.setUniformFloat(UNIFORM_SHADOW_GRID_OFFSET,
        SHADOW_GRID_OFFSET);
    m_shaderEntity.setUniformFloat(UNIFORM_SHADOW_GRID_FACTOR,
        SHADOW_GRID_FACTOR);
    m_shaderEntity.setUniformFloat(UNIFORM_SHADOW_BIAS_MIN,
        SHADOW_BIAS_MIN);
    m_shaderEntity.setUniformFloat(UNIFORM_SHADOW_BIAS_MAX,
        SHADOW_BIAS_MAX);

    // shader uniforms: depth texture and ground material
    m_materials.at(0).use(&m_shaderEntity);
    glActiveTexture(TEXTURE_UNIT_DEPTH_MAP);
    glBindTexture(GL_TEXTURE_CUBE_MAP,
        m_shadowMap.getDepthTextureID());
    m_shaderEntity.setUniformUInt(UNIFORM_SHADOW_DEPTH_TEXTURE,
        TEXTURE_INDEX_DEPTH_MAP);

    // render ground
    renderGround(&m_shaderEntity);

    // shader uniforms: entity material
    m_materials.at(1).use(&m_shaderEntity);

    // render models
    renderModels(&m_shaderEntity, deltaTime);

    // render light
    renderLight();

    // render axes and grid
    if (m_frameEnabled)
        renderFrame();
}

void Renderer::renderFrame() {
    // set line width
    glLineWidth(RENDERING_LINE_WIDTH);

    // set shader uniforms
    m_shaderFrame.use();
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());

    glBindVertexArray(m_axesVAO);
    for (GLuint i{ 0 }; i != 3; ++i) {
        // set model matrix and color
        glm::mat4 modelMatrix;
        modelMatrix *= getWorldOrientation()
            * glm::rotate(modelMatrix,
                glm::radians(ROTATION_AXES[i].first),
                ROTATION_AXES[i].second);
        m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_MODEL,
            modelMatrix);
        m_shaderFrame.setUniformVec4(UNIFORM_COLOR,
            COLOR_AXES[i]);

        // render axes
        glDrawArrays(GL_LINES, 0, 2);
    }

    // set model uniforms
    glm::mat4 modelMatrix = getWorldOrientation();
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_MODEL,
        modelMatrix);
    m_shaderFrame.setUniformVec4(UNIFORM_COLOR,
        COLOR_GRID);

    // render grid
    glBindVertexArray(m_gridVAO);
    modelMatrix = getWorldOrientation();
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_MODEL,
        modelMatrix);
    glDrawArrays(GL_LINES, 0, 404);

    // revert line width to normal
    glLineWidth(1.0f);
}

void Renderer::renderGround(Shader* shader) {
    // set shader attributes
    if (shader == &m_shaderShadow)
        m_entities.at(0).setDepthShaderAttributes(shader);
    else if (shader == &m_shaderEntity)
        m_entities.at(0).setColorShaderAttributes(shader);

    // pass model matrix to shader and render
    glm::mat4 modelMatrix = m_entities.at(0).getModelMatrix(
        getWorldOrientation());
    shader->setUniformMat4(UNIFORM_MATRIX_MODEL, modelMatrix);
    m_entities.at(0).render(m_primitive);
}

void Renderer::renderLight() {
    // set shader uniforms
    m_shaderFrame.use();
    glm::mat4 modelMatrix = getWorldOrientation()
        * glm::translate(glm::mat4(), LIGHT_POSITION);
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_MODEL, modelMatrix);
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_VIEW,
        Camera::get().getViewMatrix());
    m_shaderFrame.setUniformMat4(UNIFORM_MATRIX_PROJECTION,
        Camera::get().getProjectionMatrix());
    m_shaderFrame.setUniformVec4(UNIFORM_COLOR,
        m_lightsEnabled
            ? COLOR_LIGHT_ON
            : COLOR_LIGHT_OFF);

    // render light
    glBindVertexArray(m_lightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void Renderer::renderModels(Shader* shader, GLfloat deltaTime) {
    // cull front faces to limit peter panning
    glCullFace(GL_FRONT);

    // render models
    for (std::vector<Model>::iterator m_it{ m_models.begin() };
        m_it != m_models.end();
        ++m_it) {

        // set shader attributes
        if (shader == &m_shaderShadow)
            m_it->setDepthShaderAttributes(shader);
        else if (shader == &m_shaderEntity)
            m_it->setColorShaderAttributes(shader);

        // play animation sequence
        if (m_animationsEnabled) {
            m_animations.at(0).setSpeed(m_animationSpeedCurrent);
            m_animations.at(0).play(&(*m_it), deltaTime);
        }

        // render model hierarchy
        Model::ModelHierarchy* hierarchy = m_it->getHierarchy();
        for (Model::ModelHierarchy::const_iterator e_it{ hierarchy->begin() };
            e_it != hierarchy->end();
            ++e_it) {

            // compute entity model matrix
            glm::mat4 modelMatrix;
            modelMatrix *= glm::scale(modelMatrix,
                    e_it->first->getScalingRelative())
                * getWorldOrientation()
                * m_it->getModelMatrix(e_it->first)
                * e_it->first->getScalingMatrix();

            // set shader uniforms
            shader->use();
            shader->setUniformMat4(UNIFORM_MATRIX_MODEL,
                modelMatrix);

            // render entity
            e_it->first->render(m_primitive);
        }
    }

    // go back to culling back faces
    glCullFace(GL_BACK);
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
    if (m_modelPositions[model].x > POSITION_MAX)
        m_modelPositions[model].x = POSITION_MAX;
    else if (m_modelPositions[model].z > POSITION_MAX)
        m_modelPositions[model].z = POSITION_MAX;

    if (m_modelPositions[model].x < POSITION_MIN)
        m_modelPositions[model].x = POSITION_MIN;
    else if (m_modelPositions[model].z < POSITION_MIN)
        m_modelPositions[model].z = POSITION_MIN;
}

void Renderer::clampModelScale(GLuint model) {
    // clamp model scale
    if (m_modelScales[model].x > TRANSFORMATION_SCALE_MAX)
        m_modelScales[model] = glm::vec3(TRANSFORMATION_SCALE_MAX);
    else if (m_modelScales[model].x < TRANSFORMATION_SCALE_MIN)
        m_modelScales[model] = glm::vec3(TRANSFORMATION_SCALE_MIN);
}