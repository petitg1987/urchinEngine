# Global
* Performance
  * **OPTIMIZATION** (`minor`): Use PGO in g++
  * **OPTIMIZATION** (`minor`): Use SSE (3 dot products: Triangle3D<T>#getSupportPoint()...)

# 3d Engine
* Graphics API
  * **OPTIMIZATION** (`major`): Use bind-less rendering technique to bind almost everything at frame start (see <https://www.youtube.com/watch?v=SVm0HanVTRw> and <https://vkguide.dev/docs/gpudriven/gpu_driven_engines/>)
  * **OPTIMIZATION** (`medium`): Check secondary command buffers usage for better performance
  * **OPTIMIZATION** (`minor`): Use shader constants instead of uniform for values infrequently refreshed
  * **OPTIMIZATION** (`minor`): Use Vulkan 1.2 timeline semaphores instead of semaphores/fences
* Rendering
  * **NEW FEATURE** (`medium`): Implement a better culling technique (GPU driven rendering: <https://vkguide.dev/docs/gpudriven/gpu_driven_engines/>, coherent hierarchical culling revisited, software occlusion culling)
  * **NEW FEATURE** (`minor`): Use reverse depth for far distant view (see <https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html>)
  * **OPTIMIZATION** (`minor`): Avoid sending shader variables values at each frame when there is no change in Renderer3d#deferredRendering
* Model
  * **OPTIMIZATION** (`major`): Remove the ModelInstanceDisplayer in ModelSetDisplayer#removeModelFromDisplayer() for models not displayed for a long time
  * **OPTIMIZATION** (`medium`): Draw calls batching
    * Tips: different types of batching are possible: static, dynamic, for shadow map (see <https://docs.unity3d.com/Manual/DrawCallBatching.html>)
  * **OPTIMIZATION** (`medium`): Make UV coordinates part of the instancing
    * Tips: avoid to send full UV coordinates at each frame by using a Vulkan Descriptor Indexing to access to an array of UV by index
  * **OPTIMIZATION** (`medium`): Parallelize the creation of the ModelDisplayer
  * **OPTIMIZATION** (`medium`): Models LOD
  * **NEW FEATURE** (`medium`): Allow transparency on geometry models
* Shadow
  * **NEW FEATURE** (`major`): Shadow on omnidirectional light (check Sascha Willems: deferredshadows)
  * **OPTIMIZATION** (`medium`): Avoid to render all the frustum models for each cascade: use the split frustum models instead
  * **OPTIMIZATION** (`medium`): Create shadow map texture only for visible lights
  * **OPTIMIZATION** (`minor`): Use models LOD
  * **OPTIMIZATION** (`minor`): Create specific render passes with less restrictive memory barrier between the shadow map cascades rendering
* Lighting
  * **NEW FEATURE** (`minor`): Implement scalable ambient obscurance
* Landscape
  * **OPTIMIZATION** (`medium`): Terrain class should have methods for LOD (usable for physics and AI)
  * **NEW FEATURE** (`minor`): Use material textures (normal map...) for terrain
  * **NEW FEATURE** (`minor`): Add auto shadow on terrain
* UI
  * **OPTIMIZATION** (`major`): Remove the WidgetInstanceDisplayer in WidgetSetDisplayer#removeWidgetFromDisplayer() for widgets not displayed for a long time
  * **IMPROVEMENT** (`medium`): Add missing characters dynamically in the atlas texture(s)
  * **IMPROVEMENT** (`medium`): Dynamic scaling of characters (see <https://github.com/Chlumsky/msdfgen> or distance field font)
  * **IMPROVEMENT** (`minor`): Handle scrollable containers in UI 3d
  * **NEW FEATURE** (`minor`): Combo list
* Graphic effect
  * **NEW FEATURE** (`medium`): Decal (bullet impact, blood spread...)
  * **NEW FEATURE** (`minor`): Water transparency (see <https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh>)

# AI engine
* Navigation mesh
  * **BUG** (`major`): Surrounded faces are not walkable (NavMeshGeneratorTest#surroundedWalkableFace)
  * **BUG** (`major`): Union of room walls create wrong polygons (PolygonsUnionTest#roomPolygonsUnion)
  * **NEW FEATURE** (`major`): Add possibility to exclude dynamic objects from navigation mesh
    * Note: worth it only when steering behaviour will be implemented in pathfinding
  * **BUG** (`medium`): Jump from an edge created by an obstacle should be allowed only if target is this obstacle and vice versa
  * **BUG** (`medium`): Jump links visualization are not exact
  * **OPTIMIZATION** (`medium`): Exclude very small objects from navigation mesh
  * **NEW FEATURE** (`medium`): Create jump/drop links from an edge to a walkable surface (+ update AABBTree margin accordingly)
  * **NEW FEATURE** (`medium`): Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
  * **OPTIMIZATION** (`minor`): Reduce memory allocation in NavMeshGenerator#createNavigationPolygon()
  * **OPTIMIZATION** (`minor`): TerrainObstacleService: apply a rough simplification on self obstacles polygons
  * **NEW FEATURE** (`minor`): Exclude fast moving objects as walkable face
  * **OPTIMIZATION** (`minor`): NavMeshGenerator#computePolytopeFootprint: put result in cache
* Pathfinding
  * **NEW FEATURE** (`major`): Implement steering behaviour (see <https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777>)
  * **NEW FEATURE** (`major`): AICharacterController should refresh path points each time the path request is updated 
  * **OPTIMIZATION** (`medium`): When compute A* G score: avoid to execute funnel algorithm from start each time
  * **OPTIMIZATION** (`medium`): When search start and end triangles: use AABBox Tree algorithm

# Network engine

# Physics engine
* Broad phase
  * **OPTIMIZATION** (`medium`): Implement double AABBox trees: static and dynamic (see Bullet)
  * **OPTIMIZATION** (`medium`): Re-balance AABBox tree for better performance
* Narrow phase
  * **NEW FEATURE** (`medium`): Support joints between shapes
  * **OPTIMIZATION** (`minor`): GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
  * **OPTIMIZATION** (`minor`): EPA, add new point on convex hull to improve thanks to adjacent edges: find faster the triangles visible by the new point (see Reactphysics3d code)
* Island
  * **BUG** (`medium`): A body balancing from one side to the other side (e.g.: cone on his base) could be disabled when velocity reach zero
    * Tips: don't disable bodies when there is only one contact point
* Constraints solver
  * **BUG** (`medium`): A restitution of 1.0 introduce new force in the system. Example: a superball bounces higher and higher at each collision
* Character
  * **NEW FEATURE** (`medium`): Handle stair for character controller
  * **NEW FEATURE** (`medium`): Apply impulse on objects hit by character or falling on the character

# Sound engine
* Sound trigger
  * **NEW FEATURE** (`major`): Review spatial sound creation to avoid manual creation of the sphere shape trigger matching the spatial sound properties
  * **NEW FEATURE** (`medium`): Support convex hull sound shape
  * **NEW FEATURE** (`medium`): Support portal IN and portal OUT to trigger sound
* Listener    
  * **NEW FEATURE** (`minor`): Define a velocity to the listener

# Aggregation
* Object entity
  * **NEW FEATURE** (`major`): Attach a sound component to an object entity
  * **NEW FEATURE** (`major`): Generate collision convex hull shapes from meshes inside a urchinMesh file

# Map editor
* Render
  * **NEW FEATURE** (`medium`): When change orientation of cylinder/capsule: resize physics shape to fit the object size
  * **NEW FEATURE** (`medium`): Make objects without physics selectable with left click
  * **NEW FEATURE** (`medium`): Display box around objects which don't have mesh (allow to select them easily with left click)
* UI
  * **BUG** (`medium`): When moving the mouse to move an object: the object can sometimes move in the wrong direction
  * **NEW FEATURE** (`minor`): Allow undoing the actions
