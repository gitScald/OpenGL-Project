#include "collision.h"

std::vector<Model*> Collision::s_collisions;

void Collision::addToCollisionVector(Model* model) {
    // check whether model is already colliding
    std::vector<Model*>::iterator it = std::find(
        s_collisions.begin(),
        s_collisions.end(),
        model);

    // if not, add it
    if (it == s_collisions.end())
        s_collisions.push_back(model);
}

const std::vector<Model*> Collision::detectCollisions(
    const std::vector<Model*>& models) {
    // check for collision between pairs of models
    for (GLuint i{ 0 }; i != models.size(); ++i) {
        glm::vec3 iPosition = models.at(i)->getPosition();
        GLfloat iColliderRadius = models.at(i)->getColliderRadius();
        bool colliding{ false };

        for (GLuint j{ 0 }; j != models.size(); ++j) {
            // check if two distinct models are being checked
            if (j != i
                && !isInCollisionVector(models.at(j))) {
                glm::vec3 jPosition = models.at(j)->getPosition();
                GLfloat jCollidersRadius = models.at(j)->getColliderRadius();

                // evaluate metrics to determine if collision occurred
                glm::vec3 delta = iPosition - jPosition;
                GLfloat dist = glm::dot(delta, delta);
                GLfloat sumRadii = (iColliderRadius + jCollidersRadius)
                    * (iColliderRadius + jCollidersRadius);

                // check if model is colliding
                if (dist <= sumRadii) {
                    colliding = true;
                    break;
                }
            }
        }

        // update collision vector accordingly
        if (colliding)
            addToCollisionVector(models.at(i));
        else
            removeFromCollisionVector(models.at(i));
    }

    return s_collisions;
}

bool Collision::isInCollisionVector(Model* model) {
    // get whether model is colliding
    std::vector<Model*>::iterator it = std::find(
        s_collisions.begin(),
        s_collisions.end(),
        model);

    return it != s_collisions.end();
}

void Collision::removeFromCollisionVector(Model* model) {
    // check whether model is already colliding
    std::vector<Model*>::iterator it = std::find(
        s_collisions.begin(),
        s_collisions.end(),
        model);

    // if so, remove it
    if (it != s_collisions.end())
        s_collisions.erase(it);
}