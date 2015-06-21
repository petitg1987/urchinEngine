# UrchinEngine

Video: https://www.youtube.com/watch?v=lSbCx7u6RtE

3D Engine:
- Dynamic shadow (parallel split shadow maps)
- Dynamic lighting (deferred shading): omni-directional & directional
- Space partitioning (octree)
- File format reader: MD5 (3D model), TTF (font), TGA (bitmap)
- UI 2D: button, window, text, text input, bitmap
- Skybox
- Normal mapping

Physics Engine:
- Supported shapes: sphere, box, cylinder, capsule, convex hull
- Supported body: rigid body
- Algorithms:
-- Broadphase: sweep and prune
-- Narrowphase: sphere-sphere, sphere-box, GJK/EPA
-- Constraint solver (iterative): external force, friction, restitution, inertia
- Island

Sound Engine:
- Ambience sound and 3D sound
- Streaming
- Sound trigger in function of character position