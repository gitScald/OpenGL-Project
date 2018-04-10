#include "path.h"

void Path::addStep(PathStep* step) {
    // add path step to sequence
    m_pathSequence.push_back(step);
    if (m_stepLast < step->m_step)
        m_stepLast = step->m_step;
    m_timeTotal += step->m_time;
}

void Path::setSpeed(GLfloat speed) {
    // set pathing speed
    m_speed = speed;
}

void Path::traverse(Model* model,
    GLfloat deltaTime) {
    // update current time
    m_timeTraveled += m_speed * deltaTime;

    // update current step
    if (m_timeTraveled >= m_pathSequence.at(m_step)->m_time) {
        m_step = (m_step + 1) % (m_stepLast + 1);
        m_timeTraveled = 0.0f;
    }
    if (m_step >= m_stepLast)
        m_step = 0;

    // apply current step
    GLfloat currentOrientation = model->getOrientation();
    GLfloat targetOrientation = m_pathSequence.at(m_step)->m_direction;
    GLfloat deltaOrientation = (targetOrientation - currentOrientation)
        * m_speed
        * deltaTime;
    //model->rotate(deltaOrientation, AXIS_Y);
    model->rotate(targetOrientation, AXIS_Y);
    model->move(Transform::FRONT);
}