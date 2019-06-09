# Global
- Performance
	- (3) **OPTIMIZATION**: Use PGO in g++
	- (3) **OPTIMIZATION**: Use SSE (3 dot products: Triangle3D<T>::getSupportPoint()...)

# 3d Engine
- Lighting
	- (1) **QUALITY IMPROVEMENT**: No limit for number of light (use texture instead of uniform)
- Model
    - (1) **OPTIMIZATION**: Avoid octree resize continuously when physics object fall in nothingness
    - (2) **OPTIMIZATION**: Group same models in same octree to perform one draw call
	- (3) **OPTIMIZATION**: Models LOD
	- (3) **OPTIMIZATION**: Coherent hierarchical culling revisited
- Shadow
    - (2) **OPTIMIZATION**: Improve performance ShadowManager::updateVisibleModels
        - Tips 1: find solution where models to display could be re-used in Renderer3d::deferredGeometryRendering
        - Tips 2: call octree manager one times for all frustum splits and then split the models
	- (2) **QUALITY IMPROVEMENT**: Blur variance shadow map with 'summed area' technique.
        - Note 1: decreased light bleeding to improve quality
        - Note 2: force usage of 32 bits shadow map
	- (2) **QUALITY IMPROVEMENT**: Use mipmap on shadow map (on blured shadow maps when blur used)
	- (2) **QUALITY IMPROVEMENT**: Use anisotropic on shadow map (on blured shadow maps when blur used)
	- (3) **NEW FEATURE**: Shadow on omnidirectional light
	- (3) **NEW FEATURE**: Implement PCSS
	- (3) **OPTIMIZATION**: Use models LOD
	- (3) **OPTIMIZATION**: Create shadow map texture only for visible lights
- Terrain
    - (2) **OPTIMIZATION**: Terrain class should have methods for LOD (usable for physics and AI)
    - (2) **NEW FEATURE**: Use material textures (normal map...) for terrain
    - (2) **NEW FEATURE**: Add auto shadow on terrain
    - (2) **OPTIMIZATION**: Don't build grass quadtree which are 100% discarded by grass mask
- GUI
	- (3) **NEW FEATURE**: Combo list
	- (3) **NEW FEATURE**: Drag and drop
	- (3) **NEW FEATURE**: Textarea
	- (3) **NEW FEATURE**: Scrollbar
	- (3) **NEW FEATURE**: Text selection
	- (3) **NEW FEATURE**: Use 'glutBitmapCharacter' to display characters
- Graphic effect
	- (3) **QUALITY IMPROVEMENT**: Water (https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh)
	- (3) **NEW FEATURE**: Reflects
	- (3) **NEW FEATURE**: Smoke
	- (3) **NEW FEATURE**: Fire & explosion
	- (3) **NEW FEATURE**: Alpha management
	- (3) **OPTIMIZATION**: Textures compression

# AI engine
- Navigation mesh
	- (1) **NEW FEATURE**: Create jump links in navigation mesh
	- (3) **OPTIMIZATION**: Reduce memory allocation in NavMeshGenerator::createNavigationPolygon
	- (2) **OPTIMIZATION**: TerrainObstacleService: apply a roughly simplification on self obstacles polygons
	- (2) **OPTIMIZATION**: NavMeshGenerator#computeObstacles: select only 'expandedPolyhedrons' above 'walkableFace' with octree/AABBTree (+ reserve memory on vector 'holePolygons')
	- (2) **OPTIMIZATION**: Exclude small objects from navigation mesh
	- (1) **OPTIMIZATION**: Divide big surfaces in squares and refresh only squares
	- (3) **OPTIMIZATION**: NavMeshGenerator#computePolytopeFootprint: put result in cache
	- (3) **OPTIMIZATION**: Exclude fast moving objects from walkable face
	- (3) **QUALITY IMPROVEMENT**: Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
- Pathfinding
	- (2) **OPTIMIZATION**: When compute A* G score: avoid to execute funnel algorithm from start each time
	- (2) **OPTIMIZATION**: When search start and end triangles: use AABBox Tree algorithm
	- (1) **NEW FEATURE**: Implement steering behaviour (https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777)

# Physics engine
- Broad phase
	- (2) **OPTIMIZATION**: Implement double AABBox trees: static and dynamic (see Bullet)
	- (2) **OPTIMIZATION**: Re-balance AABBox tree for better performance
- Narrow phase
	- (2) **NEW FEATURE**: Support joints between shapes
	- (3) **OPTIMIZATION**: GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
	- (3) **OPTIMIZATION**: EPA, add new point on convex hull to improve thanks to adjacent edges: find more faster triangles visible by new point (see Reactphysics3d code)
- Island
    - (2) **BUG**: A body balancing from one side to the other side (e.g.: cone on his base) could sleep when velocity reach zero
- Constraints solver
    - (2) **BUG**: A restitution of 1.0 introduce new force in the system. Example: a superball bounces more and more higher at each collision
- Character
	- (2) **NEW FEATURE**: Handle stair for character controller
	- (3) **NEW FEATURE**: Apply impulse on objects hit by character
	
# Sound engine
- Sound trigger
	- (2) **NEW FEATURE**: Support convex hull sound shape
	- (2) **NEW FEATURE**: Support portal IN and portal OUT to trigger sound