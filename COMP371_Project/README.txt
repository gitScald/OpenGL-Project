===========================
Author:      Benjamin Vial
ID:          29590765
Course:      COMP 371
Assigment:   2
Due:         Mar 15th, 2018
===========================

C++/OpenGL assignment exploring lighting, shadows, textures, and simple animations.

Controls: As specified in the assignment guidelines, with a couple additions.

- World orientation: Press the arrow keys (LEFT/RIGHT for Ry, UP/DOWN for Rx).
- Camera panning: Holding right mouse button, drag the mouse to the left or right.
- Camera zooming: Holding left mouse button, drag the mouse up or down.
- Camera rotation (yaw/pitch): Holding middle mouse button, drag the mouse around (capped pitch value).
- Camera reset: Press the Home key.
- Model animations: Press 0-9 keys to rotate joints clockwise, holding the left shift key for counter-clockwise
    (capped rotation angles).
- Model movement: Holding the left shift key, press WASD keys (clamped to grid).
- Model random placement: Press the Spacebar.
- Model reset: Press the End key.
- Model rotation (yaw/pitch): Press the WASD keys.
- Model scaling: Press U to scale up, or J to scale down (capped value).
- Rendering primitive: Press P for GL_POINTS, L for GL_LINES, or T for GL_TRIANGLES (default).
- Lighting: Press Z to toggle (will also toggle shadows).
- Shadows: Press B to toggle.
- Textures: Press X to toggle.
- Animations: Press R to toggle.

Window resizing will not alter the objects' aspect ratio.

Archive contents:
=================

    - shaders/:                 Vertex and fragment shader files
        - entity/                 ... for the rendered entities (horse, ground, light cube)
        - frame/                  ... for the axis and grid
        - shadow/                 ... for the depth texture
    - animation.h/.cpp:         Animation class
    - animation_step.h:         AnimationStep struct
    - camera.h/.cpp:            Camera class (singleton)
    - constants.h:              Project constants
    - enums.h: Global           Project enums
    - input_manager.h/.cpp:     InputManager class (singleton)
    - joint.h/.cpp:             Model Joint class, used for animations
    - light_source.h/.cpp:      LightSource class
    - main.cpp:                 Main application source file
    - material.h/.cpp:          Material class
    - model.h/.cpp:             Hierarchical Model class
    - rendered_entity.h/.cpp:   RenderedEntity class, part of Model objects
    - renderer.h/.cpp:          Renderer class (singleton)
    - shader.h/.cpp:            Shader class
    - shadowmap.h/.cpp:         ShadowMap class, used to render depth texture
    - texture.h/.cpp:           Texture class
    - stb_image.cpp:            stb_image.h implementation for texture loading