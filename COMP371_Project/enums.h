#ifndef ENUMS_H
#define ENUMS_H

// camera displacement
namespace Eye {
    enum Displacement {
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
}

// rendering
namespace Rendering {
    // primitives
    enum Primitive {
        POINTS,
        LINES,
        TRIANGLES
    };

    // passes
    enum Pass {
        FIRST,
        SECOND
    };
}

// model transformations
namespace Transform {
    // scaling
    enum Scale {
        INCREASE,
        DECREASE
    };

    // movement and rotation
    enum Displacement {
        RANDOM,
        FRONT,
        UP,
        DOWN,
        LEFT,
        RIGHT
    };
}

// shadow calculations
namespace Shadows {
    // bias
    enum Tweak {
        INCREASE,
        DECREASE
    };
}

#endif // !ENUMS_H