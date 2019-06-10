[minor]: data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAHXRFWHRqaXJhLXN5c3RlbS1pbWFnZS10eXBlAGF2YXRhcuQCGmEAAAEVSURBVHjaY2AYPkC9+7+iTtd/ZULqtNv/q2i0/1fAkFBr/f9CreX/X43W/1MM+v8LYGjs+y+k3vp/OkgNkH6KaUDb/wr1lv+/gQr+A/FLzdb/cSDx////MwINTwHKvQbJATX/AuJSrM7T6vivDVR0EGrIf6Cmw0DNJ+H81v97dDr/axIOj9b/MVAvgTUC2Y+BYmH4daX938aQ+v8kQ/p/OxDXuOM/P1BzK1BzvV73f26oGkcgPgVUtxnTgNT/R4D4Pxin/V/EkP9fHC6X818KKLYcSf4gpgG5/9mBknVA/B2q6APQNblAdgmQ/Rkq9hWIq8FqcYKc/4pgJ8JsQ9i6FojliE9Zqf/9gBruAOlrQNqNYfgCAPlptTu9V8wIAAAAAElFTkSuQmCCieJQ5Xgm+l0+cSCHA+KJmpXCFecl3vzyk/8N0FuPTDE+xpsN9kUab/SqzvoXqzl2J38+PwAAAAAASUVORK5CYII=
[medium]: data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAHXRFWHRqaXJhLXN5c3RlbS1pbWFnZS10eXBlAGF2YXRhcuQCGmEAAAEHSURBVHjaY2AYvuB/mKEdEJ/5H2Zw/n+EiQPxGmOMJf+HGywBavyPgsMNl4LkcGtscGD5H2FUCFT8CaLJ8BtQU93/cKNqMBtiyEeg4fn/V4UyYxoQbrAPybYN/yMsFOByQDZYDCG/A4sBhoeAhlwB0p4QFyVw/C8LLv1fGlzxvy+UE2KQsTdQ/hoQ78EfDuXBvv9Lgu/9Lwn6D8alQff/l4X6EQ7AimAloIYtCI3BF4H4PBJ/8/+aUEXsmsuC84AKvkMVfgDzgYEFxqUhuWAxiNz3/2UhWZgGlAS9BuJ/QKcv+F8VKY4hDxQDyi2CuuYZpgGVoVr/y0N1CXqzJFjvf2WwxjDKMwDp4MDWAYTclAAAAABJRU5ErkJggicoFECtUiW7BqT4TYzZi9fUNYe7hjIGVgIOdj4KWCChNuBwjNHsdiIjMiqP9xYimVyUbkDGe1raLTZw62fArjZKWJXmY5cG8X2lDQc5qC3BjuR3/h/PHyU/eDdI9YdLAAAAAElFTkSuQmCC
[major]: data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAHXRFWHRqaXJhLXN5c3RlbS1pbWFnZS10eXBlAGF2YXRhcuQCGmEAAACISURBVHjaY2CgNvifZM37P9yw8X+YwXkg/gzF58FiQDn8miONnIAKHwA1/MeKQXJANbg1hxn8w6kZgf9hGAJ19gMiNCNcguwdqJ//k4SBepANuECyAcCARRgACWmQoBfBWIow9oaq/YxpAFCSXAPOU+YFigOR0mikOCFRJSljZCZI2iAtM5EDAM05coChdTukAAAAAElFTkSuQmCCDpzjcrMeIqLnmxxMveIc3ZCvwGKS9K8BdrhtBQAVRzJhpqesk9Q6MLV3BeNmICKZag5jW2CF6WrGktg1FpKuBCpPVhj7NIKDVI1O0mGIqixW2GAlcnNDhXJZZ+rhMYk2ftfb+5hGVDyc8gPFfWG3WJqRxwAAAABJRU5ErkJggg==

# Global
- Performance
	- **OPTIMIZATION** (![minor]): Use PGO in g++
	- **OPTIMIZATION** (![minor]): Use SSE (3 dot products: Triangle3D<T>::getSupportPoint()...)

# 3d Engine
- Lighting
	- **QUALITY IMPROVEMENT** (![major]): No limit for number of light (use texture instead of uniform)
- Model
    - **OPTIMIZATION** (![major]): Avoid octree resize continuously when physics object fall in nothingness
    - **OPTIMIZATION** (![medium]): Group same models in same octree to perform one draw call
	- **OPTIMIZATION** (![minor]): Models LOD
	- **OPTIMIZATION** (![minor]): Coherent hierarchical culling revisited
- Shadow
    - **OPTIMIZATION** (![medium]): Improve performance ShadowManager::updateVisibleModels
        - Tips 1: find solution where models to display could be re-used in Renderer3d::deferredGeometryRendering
        - Tips 2: call octree manager one times for all frustum splits and then split the models
	- **QUALITY IMPROVEMENT** (![medium]): Blur variance shadow map with 'summed area' technique.
        - Note 1: decreased light bleeding to improve quality
        - Note 2: force usage of 32 bits shadow map
	- **QUALITY IMPROVEMENT** (![medium]): Use mipmap on shadow map (on blurred shadow maps when blur used)
	- **QUALITY IMPROVEMENT** (![medium]): Use anisotropic on shadow map (on blurred shadow maps when blur used)
	- **NEW FEATURE** (![minor]): Shadow on omnidirectional light
	- **NEW FEATURE** (![minor]): Implement PCSS
	- **OPTIMIZATION** (![minor]): Use models LOD
	- **OPTIMIZATION** (![minor]): Create shadow map texture only for visible lights
- Terrain
    - **OPTIMIZATION** (![medium]): Terrain class should have methods for LOD (usable for physics and AI)
    - **NEW FEATURE** (![medium]): Use material textures (normal map...) for terrain
    - **NEW FEATURE** (![medium]): Add auto shadow on terrain
    - **OPTIMIZATION** (![medium]): Don't build grass quadtree which are 100% discarded by grass mask
- GUI
	- **NEW FEATURE** (![minor]): Combo list
	- **NEW FEATURE** (![minor]): Drag and drop
	- **NEW FEATURE** (![minor]): Textarea
	- **NEW FEATURE** (![minor]): Scrollbar
	- **NEW FEATURE** (![minor]): Text selection
	- **NEW FEATURE** (![minor]): Use 'glutBitmapCharacter' to display characters
- Graphic effect
	- **QUALITY IMPROVEMENT** (![minor]): Water (https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh)
	- **NEW FEATURE** (![minor]): Reflects
	- **NEW FEATURE** (![minor]): Smoke
	- **NEW FEATURE** (![minor]): Fire & explosion
	- **NEW FEATURE** (![minor]): Alpha management
	- **OPTIMIZATION** (![minor]): Textures compression

# AI engine
- Navigation mesh
	- **NEW FEATURE** (![major]): Create jump links in navigation mesh
	- **OPTIMIZATION** (![minor]): Reduce memory allocation in NavMeshGenerator::createNavigationPolygon
	- **OPTIMIZATION** (![medium]): TerrainObstacleService: apply a roughly simplification on self obstacles polygons
	- **OPTIMIZATION** (![medium]): NavMeshGenerator#computeObstacles: select only 'expandedPolyhedrons' above 'walkableFace' with octree/AABBTree (+ reserve memory on vector 'holePolygons')
	- **OPTIMIZATION** (![medium]): Exclude small objects from navigation mesh
	- **OPTIMIZATION** (![major]): Divide big surfaces in squares and refresh only squares
	- **OPTIMIZATION** (![minor]): NavMeshGenerator#computePolytopeFootprint: put result in cache
	- **OPTIMIZATION** (![minor]): Exclude fast moving objects from walkable face
	- **QUALITY IMPROVEMENT** (![minor]): Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
- Pathfinding
	- **OPTIMIZATION** (![medium]): When compute A* G score: avoid to execute funnel algorithm from start each time
	- **OPTIMIZATION** (![medium]): When search start and end triangles: use AABBox Tree algorithm
	- **NEW FEATURE** (![major]): Implement steering behaviour (https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777)

# Physics engine
- Broad phase
	- **OPTIMIZATION** (![medium]): Implement double AABBox trees: static and dynamic (see Bullet)
	- **OPTIMIZATION** (![medium]): Re-balance AABBox tree for better performance
- Narrow phase
	- **NEW FEATURE** (![medium]): Support joints between shapes
	- **OPTIMIZATION** (![minor]): GJK, don't test voronoi region opposite to last point added (2D: A, B, AB | 3D: ABC)
	- **OPTIMIZATION** (![minor]): EPA, add new point on convex hull to improve thanks to adjacent edges: find more faster triangles visible by new point (see Reactphysics3d code)
- Island
    - **BUG** (![medium]): A body balancing from one side to the other side (e.g.: cone on his base) could sleep when velocity reach zero
- Constraints solver
    - **BUG** (![medium]): A restitution of 1.0 introduce new force in the system. Example: a superball bounces more and more higher at each collision
- Character
	- **NEW FEATURE** (![medium]): Handle stair for character controller
	- **NEW FEATURE** (![minor]): Apply impulse on objects hit by character
	
# Sound engine
- Sound trigger
	- **NEW FEATURE** (![medium]): Support convex hull sound shape
	- **NEW FEATURE** (![medium]): Support portal IN and portal OUT to trigger sound