# Global
* Performance
  * ▼ **OPTIMIZATION**: Use PGO in g++

# 3d Engine
* Graphics API
  * ► **OPTIMIZATION**: Use bind-less rendering technique to bind almost everything at frame start
    * See: <https://www.youtube.com/watch?v=SVm0HanVTRw>
    * See: <https://vkguide.dev/docs/gpudriven/gpu_driven_engines/>
    * See: <https://anki3d.org/resource-uniformity-bindless-access-in-vulkan/>
  * ► **OPTIMIZATION**: Use sub-passes for deferred rendering
    * See: <https://www.saschawillems.de/blog/2018/07/19/vulkan-input-attachments-and-sub-passes/>
  * ► **OPTIMIZATION**: Check secondary command buffers usage for better performance
  * ▼ **OPTIMIZATION**: Use Vulkan 1.2 timeline semaphores instead of semaphores/fences
* Rendering
  * ► **NEW FEATURE**: Implement a better culling technique like GPU driven rendering, coherent hierarchical culling revisited or software occlusion culling
    * GPU driven rendering: <https://vkguide.dev/docs/gpudriven/gpu_driven_engines/>
  * ► **NEW FEATURE**: Use sRGB format for color/albedo framebuffers and swap chain
* Model
  * ► **OPTIMIZATION**: Implement draw calls batching: static, dynamic or specific for shadow map
    * See: <https://docs.unity3d.com/Manual/DrawCallBatching.html>
  * ► **OPTIMIZATION**: Parallelize the creation of the ModelDisplayer
  * ► **OPTIMIZATION**: Models LOD
* Lighting & Shadow
  * ▲ **NEW FEATURE**: Implement a 'light blocking volume' to restrict lights without shadow to the current room
  * ▲ **OPTIMIZATION**: Free shadow map memory when light is not displayed on the scene
  * ► **IMPROVEMENT**: Use distance to light instead of depth in spot/omnidirectional shadow map: allow to avoid artifacts due to wrong bias
    * See: <https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows>
  * ▼ **OPTIMIZATION**: Create specific render passes with less restrictive memory barrier between the shadow map cascades rendering
  * ▼ **NEW FEATURE**: Implement scalable ambient obscurance
* Anti-aliasing
  * ► **NEW FEATURE**: Handling moving objects in TAA
    * See: <https://www.elopezr.com/temporal-aa-and-the-quest-for-the-holy-trail/>
  * ▼ **OPTIMIZATION**: Use computer shaders
* Landscape
  * ► **OPTIMIZATION**: Terrain class should have methods for LOD (usable for physics and AI)
  * ▼ **NEW FEATURE**: Use material textures (normal map...) for terrain
  * ▼ **NEW FEATURE**: Add auto shadow on terrain
  * ▼ **NEW FEATURE**: Water transparency
    * See: <https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh>
* UI
  * ► **NEW FEATURE**: Render UI into texture to avoid depth shift in 3d UI
  * ► **IMPROVEMENT**: Add missing characters dynamically in the atlas texture(s)
  * ► **IMPROVEMENT**: Implement dynamic scaling of characters like multichannel signed distance or distance field font
    * Multichannel signed distance: <https://github.com/Chlumsky/msdfgen>
  * ▼ **IMPROVEMENT**: Handle scrollable containers in UI 3d
  * ▼ **NEW FEATURE**: Combo list

# Physics engine
* Broad phase
  * ► **OPTIMIZATION**: Implement double AABBox trees: static and dynamic (see Bullet)
  * ► **OPTIMIZATION**: Re-balance AABBox tree for better performance
* Narrow phase
  * ► **NEW FEATURE**: Support joints between shapes
  * ▼ **OPTIMIZATION**: GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
  * ▼ **OPTIMIZATION**: EPA, add new point on convex hull to improve thanks to adjacent edges: find faster the triangles visible by the new point (see Reactphysics3d code)
* Island
  * ► **BUG**: A body balancing from one side to the other side (e.g.: cone on his base) could be disabled when velocity reach zero
    * Tips: don't disable bodies when there is only one contact point
* Constraints solver
  * ► **BUG**: A restitution of 1.0 introduce new force in the system. Example: a superball bounces higher and higher at each collision
    * See <https://youtu.be/ZOfMA8h4qO8?t=109> for possible cause
* Character
  * ► **NEW FEATURE**: Handle stair for character controller
  * ► **NEW FEATURE**: Apply impulse on objects hit by character or falling on the character

# Sound engine
* Sound trigger
  * ► **NEW FEATURE**: Support convex hull sound shape
  * ► **NEW FEATURE**: Support portal IN and portal OUT to trigger sound
* Listener    
  * ▼ **NEW FEATURE**: Define a velocity to the listener

# Network engine
* Multiplayer
  * ► **NEW FEATURE**: Support for multiplayer

# AI engine
* Navigation mesh
  * ▲ **NEW FEATURE**: Generate the navigation mesh
* Pathfinding
  * ▲ **NEW FEATURE**: Implement steering behaviour
    * See: <https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777>
  * ▲ **NEW FEATURE**: AICharacterController should refresh path points each time the path request is updated 
  * ► **OPTIMIZATION**: When compute A* G score: avoid to execute funnel algorithm from start each time
  * ► **OPTIMIZATION**: When search start and end triangles: use AABBox Tree algorithm

# Aggregation
* `None`

# Map editor
* Physics
  * ► **NEW FEATURE**: Make objects without physics selectable with left click
  * ► **NEW FEATURE**: Display box around objects which don't have mesh (allow to select them easily with left click)
* UI
  * ▼ **NEW FEATURE**: Allow undoing the actions
