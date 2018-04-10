#ifndef COLLISION_H
#define COLLISION_H

#include "model.h"

class Collision {
public:
    Collision() = delete;
    Collision(const Collision& collision) = delete;
    Collision(Collision&& collision) = delete;
    Collision& operator=(Collision& collision) = delete;

    // collision detection
    static const std::vector<Model*> detectCollisions(
        const std::vector<Model*>& models);

private:
    static std::vector<Model*> s_collisions;

    // collision handling
    static void addToCollisionVector(Model* model);
    static bool isInCollisionVector(Model* model);
    static void removeFromCollisionVector(Model* model);
};

#endif // !COLLISION_H
