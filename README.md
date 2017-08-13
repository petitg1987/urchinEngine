# UrchinEngine
Game engine library: 3D, Physics, Sound, IA, Map Editor

Video: https://www.youtube.com/watch?v=lSbCx7u6RtE

- 3D Engine:
  - Dynamic shadow (cached parallel split shadow maps, variance shadow map, blur)
  - Dynamic lighting (deferred shading): omni-directional & directional
  - Animate 3d models reader
  - Normal mapping
  - Ambiant occlusion (HBAO)
  - Anti-aliasing (FXAA)
  - Space partitioning (octree)
  - UI 2D: button, window, text (TTF reader), text input, bitmap (TGA reader), slider
  - Skybox

- Physics Engine:
  - Supported shapes: sphere, box, cylinder, capsule, cone, convex hull, compound shape
  - Supported body: rigid body
  - Algorithms:
    - Broadphase: sweep and prune & AABB Tree
    - Narrowphase: sphere-sphere, sphere-box, GJK/EPA
    - Constraint solver (iterative): external force, friction, restitution, inertia
  - Island
  - Ray cast

- Sound Engine:
  - Ambience sound and 3D sound
  - Streaming
  - Sound trigger in function of character position

- IA Engine:
  - Automatic navigation mesh generator
  
- Map Editor:
  - Add > 3d model, rigid body and sound on scene
  - Debug > view light scope, hitbox, sound trigger, navigation mesh
