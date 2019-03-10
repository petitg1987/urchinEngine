# To do
- Navigation mesh
	- (1) **NEW FEATURE**: Create jump links in navigation mesh
	- (1) **OPTIMIZATION**: Reduce memory allocation in NavMeshGenerator::createNavigationPolygon
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

# Known bugs
- 

# Notes
- 
