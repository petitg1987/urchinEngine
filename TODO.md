# Global
* Performance
  * **OPTIMIZATION** (`minor`): Use PGO in g++
  * **OPTIMIZATION** (`minor`): Use SSE (3 dot products: Triangle3D<T>#getSupportPoint()...)

# 3d Engine
* General
  * **NEW FEATURE** (`major`): Replace geometry shader by instantiation
  * **NEW FEATURE** (`major`): Replace OpenGL by Vulkan
* Rendering
  * **OPTIMIZATION** (`minor`): Avoid sending shader variables values at each frame when there is no change in Renderer3d#deferredGeometryRendering()
  * **NEW FEATURE** (`minor`): Alpha management
  * **OPTIMIZATION** (`minor`): Textures compression  
* Model
  * **OPTIMIZATION** (`major`): Use instantiation mechanism when identical models are displayed several times in the scene
  * **OPTIMIZATION** (`medium`): Regroup draw calls when possible
  * **NEW FEATURE** (`minor`): Use reverse depth for far distant view (see <https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html>)
  * **OPTIMIZATION** (`medium`): Models LOD
  * **OPTIMIZATION** (`minor`): Subdivide octree only when number of objects inside this octree reach a threshold
  * **OPTIMIZATION** (`minor`): Coherent hierarchical culling revisited
* Shadow
  * **OPTIMIZATION** (`medium`): Improve performance ShadowManager::updateVisibleModels
    * Tips 1: find solution where models to display could be re-used in Renderer3d#deferredGeometryRendering()
    * Tips 2: call octree manager one times for all frustum splits and then split the models
  * **NEW FEATURE** (`minor`): Blur variance shadow map with 'summed area' technique.
    * Note 1: decreased light bleeding to improve quality
    * Note 2: force usage of 32 bits shadow map
  * **NEW FEATURE** (`medium`): Use mipmap on shadow map (on blurred shadow maps when blur used)
  * **NEW FEATURE** (`minor`): Use anisotropic on shadow map (on blurred shadow maps when blur used)
  * **NEW FEATURE** (`major`): Shadow on omnidirectional light
  * **NEW FEATURE** (`minor`): Implement PCSS
  * **OPTIMIZATION** (`minor`): Use models LOD
  * **OPTIMIZATION** (`medium`): Create shadow map texture only for visible lights
* Terrain
  * **OPTIMIZATION** (`medium`): Terrain class should have methods for LOD (usable for physics and AI)
  * **NEW FEATURE** (`medium`): Use material textures (normal map...) for terrain
  * **NEW FEATURE** (`minor`): Add auto shadow on terrain
  * **OPTIMIZATION** (`medium`): Don't build grass quadtree which are 100% discarded by grass mask
* GUI
  * **NEW FEATURE** (`minor`): Combo list
  * **NEW FEATURE** (`minor`): Drag and drop
  * **NEW FEATURE** (`minor`): Textarea
  * **NEW FEATURE** (`medium`): Scrollbar
  * **NEW FEATURE** (`medium`): Text selection
  * **BUG** (`critical`): Widgets are not screen resolution independent
* Graphic effect
  * **NEW FEATURE** (`minor`): Water transparency (see <https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh>)
  * **NEW FEATURE** (`minor`): Reflects
  * **NEW FEATURE** (`minor`): Smoke
  * **NEW FEATURE** (`minor`): Fire & explosion

# AI engine
* Navigation mesh
  * **BUG** (`medium`): Jump from an edge created by an obstacle should be allowed only if target is this obstacle and vice versa
  * **NEW FEATURE** (`medium`): Create jump/drop links from an edge to a walkable surface (+ update AABBTree margin accordingly)
  * **NEW FEATURE** (`major`): Add possibility to exclude dynamic objects from navigation mesh
    * Note: worth it only when steering behaviour will be implemented in pathfinding
  * **OPTIMIZATION** (`minor`): Reduce memory allocation in NavMeshGenerator#createNavigationPolygon()
  * **OPTIMIZATION** (`minor`): TerrainObstacleService: apply a roughly simplification on self obstacles polygons
  * **OPTIMIZATION** (`medium`): Exclude very small objects from navigation mesh
  * **NEW FEATURE** (`minor`): Exclude fast moving objects as walkable face
  * **OPTIMIZATION** (`minor`): NavMeshGenerator#computePolytopeFootprint: put result in cache
  * **NEW FEATURE** (`medium`): Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
- Pathfinding
  * **OPTIMIZATION** (`medium`): When compute A* G score: avoid to execute funnel algorithm from start each time
  * **OPTIMIZATION** (`medium`): When search start and end triangles: use AABBox Tree algorithm
  * **NEW FEATURE** (`major`): Implement steering behaviour (see <https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777>)
  * **NEW FEATURE** (`critical`): AICharacterController should refresh path points each time the path request is updated 

# Network engine
* Requests
  * **NEW FEATURE** (`medium`): Make HTTP requests calls thread-safe

# Physics engine
* Broad phase
  * **OPTIMIZATION** (`medium`): Implement double AABBox trees: static and dynamic (see Bullet)
  * **OPTIMIZATION** (`medium`): Re-balance AABBox tree for better performance
* Narrow phase
  * **NEW FEATURE** (`medium`): Support joints between shapes
  * **OPTIMIZATION** (`minor`): GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
  * **OPTIMIZATION** (`minor`): EPA, add new point on convex hull to improve thanks to adjacent edges: find more faster triangles visible by new point (see Reactphysics3d code)
* Island
  * **BUG** (`major`): A body balancing from one side to the other side (e.g.: cone on his base) could disabled when velocity reach zero
    * Tips 1: don't disable bodies when there is only one contact point
* Constraints solver
  * **BUG** (`medium`): A restitution of 1.0 introduce new force in the system. Example: a superball bounces more and more higher at each collision
* Character
  * **BUG** (`critical`): On slow system, the character can go through the ground at the application start
  * **BUG** (`major`): User jump actions are sometimes ignored when character moves on a terrain
  * **NEW FEATURE** (`medium`): Handle stair for character controller
  * **NEW FEATURE** (`medium`): Apply impulse on objects hit by character or falling on the character

# Sound engine
* Sound trigger
  * **NEW FEATURE** (`medium`): Support convex hull sound shape
  * **NEW FEATURE** (`major`): Support portal IN and portal OUT to trigger sound

# Map editor
* Render
  * **NEW FEATURE** (`medium`): When change orientation of cylinder/capsule: resize physics shape to fit the object size
  * **NEW FEATURE** (`medium`): Make objects without physics selectable with left click
  * **NEW FEATURE** (`medium`): Display box around objects which don't have mesh (allow to select them easily with left click)
* UI
  * **NEW FEATURE** (`minor`): Allow undo on actions
