# Global
- Performance
	- **OPTIMIZATION** (`minor`): Use PGO in g++
	- **OPTIMIZATION** (`minor`): Use SSE (3 dot products: Triangle3D<T>::getSupportPoint()...)

# 3d Engine
- Model
    - **OPTIMIZATION** (`medium`): Group same models in same octree to perform one draw call
    - **NEW FEATURE** (`minor`): Use reverse depth for far distant view (<https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html>)
	- **OPTIMIZATION** (`minor`): Models LOD
	- **OPTIMIZATION** (`minor`): Subdivide octree only when number of objects inside this octree reach a threshold
	- **OPTIMIZATION** (`minor`): Coherent hierarchical culling revisited
- Lighting
    - **OPTIMIZATION** (`minor`): Use tiled-based deferred shading (<https://software.intel.com/en-us/articles/deferred-rendering-for-current-and-future-rendering-pipelines>)
- Shadow
    - **OPTIMIZATION** (`medium`): Improve performance ShadowManager::updateVisibleModels
        - Tips 1: find solution where models to display could be re-used in Renderer3d::deferredGeometryRendering
        - Tips 2: call octree manager one times for all frustum splits and then split the models
	- **QUALITY IMPROVEMENT** (`medium`): Blur variance shadow map with 'summed area' technique.
        - Note 1: decreased light bleeding to improve quality
        - Note 2: force usage of 32 bits shadow map
	- **QUALITY IMPROVEMENT** (`medium`): Use mipmap on shadow map (on blurred shadow maps when blur used)
	- **QUALITY IMPROVEMENT** (`medium`): Use anisotropic on shadow map (on blurred shadow maps when blur used)
	- **NEW FEATURE** (`minor`): Shadow on omnidirectional light
	- **NEW FEATURE** (`minor`): Implement PCSS
	- **OPTIMIZATION** (`minor`): Use models LOD
	- **OPTIMIZATION** (`minor`): Create shadow map texture only for visible lights
- Terrain
    - **OPTIMIZATION** (`medium`): Terrain class should have methods for LOD (usable for physics and AI)
    - **NEW FEATURE** (`medium`): Use material textures (normal map...) for terrain
    - **NEW FEATURE** (`medium`): Add auto shadow on terrain
    - **OPTIMIZATION** (`medium`): Don't build grass quadtree which are 100% discarded by grass mask
- GUI
	- **NEW FEATURE** (`minor`): Combo list
	- **NEW FEATURE** (`minor`): Drag and drop
	- **NEW FEATURE** (`minor`): Textarea
	- **NEW FEATURE** (`minor`): Scrollbar
	- **NEW FEATURE** (`minor`): Text selection
	- **NEW FEATURE** (`minor`): Use 'glutBitmapCharacter' to display characters
- Graphic effect
	- **QUALITY IMPROVEMENT** (`minor`): Water (<https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh>)
	- **NEW FEATURE** (`minor`): Reflects
	- **NEW FEATURE** (`minor`): Smoke
	- **NEW FEATURE** (`minor`): Fire & explosion
	- **NEW FEATURE** (`minor`): Alpha management
	- **OPTIMIZATION** (`minor`): Textures compression

# AI engine
- Navigation mesh
	- **BUG** (`medium`): Jump from an edge created by an obstacle should be allowed only if target is this obstacle and vice versa
	- **NEW FEATURE** (`medium`): Create jump/drop links from an edge to a walkable surface (+ update AABBTree margin accordingly)
	- **OPTIMIZATION** (`minor`): Reduce memory allocation in NavMeshGenerator::createNavigationPolygon
	- **OPTIMIZATION** (`minor`): TerrainObstacleService: apply a roughly simplification on self obstacles polygons
	- **OPTIMIZATION** (`medium`): Exclude small objects from navigation mesh
	- **OPTIMIZATION** (`minor`): Exclude fast moving objects from walkable face
	- **OPTIMIZATION** (`minor`): NavMeshGenerator#computePolytopeFootprint: put result in cache
	- **QUALITY IMPROVEMENT** (`minor`): Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
- Pathfinding
	- **OPTIMIZATION** (`medium`): When compute A* G score: avoid to execute funnel algorithm from start each time
	- **OPTIMIZATION** (`medium`): When search start and end triangles: use AABBox Tree algorithm
	- **NEW FEATURE** (`major`): Implement steering behaviour (<https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777>)

# Physics engine
- Broad phase
	- **OPTIMIZATION** (`medium`): Implement double AABBox trees: static and dynamic (see Bullet)
	- **OPTIMIZATION** (`medium`): Re-balance AABBox tree for better performance
- Narrow phase
	- **NEW FEATURE** (`medium`): Support joints between shapes
	- **OPTIMIZATION** (`minor`): GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
	- **OPTIMIZATION** (`minor`): EPA, add new point on convex hull to improve thanks to adjacent edges: find more faster triangles visible by new point (see Reactphysics3d code)
- Island
    - **BUG** (`medium`): A body balancing from one side to the other side (e.g.: cone on his base) could sleep when velocity reach zero
- Constraints solver
    - **BUG** (`medium`): A restitution of 1.0 introduce new force in the system. Example: a superball bounces more and more higher at each collision
- Character
	- **NEW FEATURE** (`medium`): Handle stair for character controller
	- **NEW FEATURE** (`minor`): Apply impulse on objects hit by character
	
# Sound engine
- Sound trigger
	- **NEW FEATURE** (`medium`): Support convex hull sound shape
	- **NEW FEATURE** (`medium`): Support portal IN and portal OUT to trigger sound
	
# Map editor
- Render
	- **NEW FEATURE** (`major`): Allow to move objects with mouse
	- **QUALITY IMPROVEMENT** (`medium`): When change orientation of cylinder/capsule: resize physics shape to fit the object size
	- **QUALITY IMPROVEMENT** (`medium`): Make objects without physics selectable with left click
	- **NEW FEATURE** (`medium`): Display box around objects which don't have mesh (allow to select them easily with left click)
- UI
	- **NEW FEATURE** (`minor`): Allow CTRL+Y to return in previous value
	- **NEW FEATURE** (`medium`): When save (CTRL+S): change window title from "(not saved)" to "(saved)"
