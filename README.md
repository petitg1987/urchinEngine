<h2 align="center">Urchin Engine</h2>

# Videos
* Game created with the engine (2023): <https://www.youtube.com/watch?v=JbVsi7y7nu0>
* Technical demo of physics and shadows (2015): <https://www.youtube.com/watch?v=lSbCx7u6RtE>

# Description
Game engine library in C++20: 3D, Physics, Sound, Network, IA, Map Editor

* 3D Engine:
  * Graphic API: Vulkan
  * 3D models with skeleton animation & Blender exporter
  * Materials:
    * Albedo texture or color
    * Normal texture
    * PBR with metal/roughness workflow
    * Transparency
    * Ambient
    * Emissive
    * Depth test/write
  * Dynamic shadow: cascade shadow maps + improved PCF filter + stabilized shadow
  * Dynamic lighting with deferred shading: directional, omnidirectional, spot
  * Terrain multi-layered and grass
  * Ambient occlusion (SSAO+)
  * Anti-aliasing (TAA and FXAA)
  * Space partitioning: octree
  * UI: text, button, window, textbox, textarea, bitmap, sequence, checkbox, slider, scrollbar
  * Skybox
  * Fog

* Physics Engine:
  * Supported shapes: sphere, box, cylinder, capsule, cone, convex hull, compound shape, terrain
  * Body types: rigid body, ghost body
  * Algorithms:
    * Broad phase: AABBox Tree
    * Narrow phase: sphere-sphere, sphere-box, GJK/EPA, concave-convex
    * Constraint solver (iterative): external force, friction, restitution, inertia
  * Continuous collision detection (CCD)
  * Island
  * Ray cast
  * Character controller

* Sound Engine:
  * Supported formats: ogg
  * Localizable sound and global sound
  * Sound trigger based on character position

* IA Engine:
  * Pathfinding A* with funnel algorithm
  * Character controller
  
* Network Engine:
  * HTTP request sender
  
* Map Editor:
  * Handle:
    * Objects: 3d models, physics, lights, sound
    * Landscape: terrain, water, sky
    * AI agent
  * Debug visualization: hit box, light range, sound trigger, navigation mesh

# Build
* Environment setup: [here](./DEV.md)
* Build:
  * Debug:
    ```
    git clone https://github.com/petitg1987/urchinEngine.git && cd ./urchinEngine
    cmake -DCMAKE_BUILD_TYPE=Debug -S ./ -B .build/debug/
    cmake --build .build/debug/ -- -j 16
    ```
  * Release:
    ```
    git clone https://github.com/petitg1987/urchinEngine.git && cd ./urchinEngine
    cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -S ./ -B .build/release/
    cmake --build .build/release/ -- -j 16
    ```
* Execute tests:
  ```
  cd urchinEngine/test/
  ./testExecutor [unit] [integration] [monkey]
  ```
* Launch map editor:
  ```
  cd urchinEngine/mapEditor/
  ./urchinMapEditor
  ```
