# Global
* Performance
  * **OPTIMIZATION** (`minor`): Use PGO in g++
  * **OPTIMIZATION** (`minor`): Use SSE (3 dot products: Triangle3D<T>#getSupportPoint()...)
* Setup
  * **NEW FEATURE** (`major`): Update DEV.md to make glslc available in mingw64.exe shell

# 3d Engine
* Graphics API
  * **OPTIMIZATION** (`minor`): Use shader constants (VkPipelineShaderStageCreateInfo.pSpecializationInfo) instead of uniform for values infrequently refreshed
  * **OPTIMIZATION** (`minor`): Update descriptor sets (GenericRenderer.updateDescriptorSets) with updated values only
  * **OPTIMIZATION** (`medium`): Check secondary command buffers usage for better performance
  * **OPTIMIZATION** (`minor`): Use Vulkan 1.2 timeline semaphores instead of semaphores/fences
* Rendering
  * **NEW FEATURE** (`major`): Replace geometry shaders by instantiation
  * **NEW FEATURE** (`minor`): Use reverse depth for far distant view (see <https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html>)
  * **OPTIMIZATION** (`minor`): Avoid sending shader variables values at each frame when there is no change in Renderer3d#deferredRendering()
  * **OPTIMIZATION** (`minor`): Subdivide octree only when number of objects inside this octree reach a threshold
  * **OPTIMIZATION** (`minor`): Coherent hierarchical culling revisited
  * **OPTIMIZATION** (`minor`): Software occlusion culling
* Model
  * **OPTIMIZATION** (`medium`): Use instantiation mechanism when identical models are displayed several times in the scene
  * **OPTIMIZATION** (`medium`): Draw calls batching
    * Tips 1: different types of batching are possible: static, dynamic, for shadow map (see <https://docs.unity3d.com/Manual/DrawCallBatching.html>)
  * **NEW FEATURE** (`medium`): Animation blending
  * **OPTIMIZATION** (`major`): Clean ModelDisplayer in ModelSetDisplayer for models not displayed for a long time
  * **OPTIMIZATION** (`major`): Parallelize the creation of the ModelDisplayer
  * **OPTIMIZATION** (`medium`): Models LOD
* Geometry model 
  * **NEW FEATURE** (`medium`): Allow transparency on geometry models
* Shadow
  * **OPTIMIZATION** (`major`): Improve performance of ShadowManager::updateVisibleModels and Renderer3d::updateModelsInFrustum
    * Tips 1: find solution where models could be re-used in the second method
    * Tips 2: call octree manager one times for all frustum splits and then split the models
  * **NEW FEATURE** (`minor`): Blur variance shadow map with 'summed area' technique
    * Note 1: decreased light bleeding to improve quality
    * Note 2: force usage of 32 bits shadow map
  * **NEW FEATURE** (`medium`): Use mipmap on shadow map (on blurred shadow maps when blur used)
  * **NEW FEATURE** (`minor`): Use anisotropic on shadow map (on blurred shadow maps when blur used)
  * **NEW FEATURE** (`major`): Shadow on omnidirectional light (check Sascha Willems: deferredshadows)
  * **NEW FEATURE** (`minor`): Implement PCSS
  * **OPTIMIZATION** (`minor`): Use models LOD
  * **OPTIMIZATION** (`medium`): Create shadow map texture only for visible lights
  * **OPTIMIZATION** (`medium`): Check if fields of VkPipelineRasterizationStateCreateInfo can improve the performances (check Sascha Willems: deferredshadows)
* Ambient occlusion
  * **NEW FEATURE** (`minor`): Implement scalable ambient obscurance
* Landscape
  * **OPTIMIZATION** (`medium`): Terrain class should have methods for LOD (usable for physics and AI)
  * **NEW FEATURE** (`medium`): Use material textures (normal map...) for terrain
  * **NEW FEATURE** (`minor`): Add auto shadow on terrain
  * **OPTIMIZATION** (`medium`): Do not build grass quadtree which are 100% discarded by grass mask
* UI
  * **IMPROVEMENT** (`medium`): Add missing characters dynamically in the atlas texture(s)
  * **IMPROVEMENT** (`medium`): Dynamic scaling of characters (see <https://github.com/Chlumsky/msdfgen>)
  * **NEW FEATURE** (`minor`): Combo list
  * **NEW FEATURE** (`minor`): Textarea
  * **NEW FEATURE** (`medium`): Text selection
* Graphic effect
  * **NEW FEATURE** (`minor`): Water transparency (see <https://www.youtube.com/watch?v=HusvGeEDU_U&list=PLRIWtICgwaX23jiqVByUs0bqhnalNTNZh>)
  * **NEW FEATURE** (`minor`): Smoke
  * **NEW FEATURE** (`medium`): Decal (bullet impact, blood spread...)

# AI engine
* Navigation mesh
  * **BUG** (`medium`): Jump from an edge created by an obstacle should be allowed only if target is this obstacle and vice versa
  * **NEW FEATURE** (`medium`): Create jump/drop links from an edge to a walkable surface (+ update AABBTree margin accordingly)
  * **NEW FEATURE** (`major`): Add possibility to exclude dynamic objects from navigation mesh
    * Note: worth it only when steering behaviour will be implemented in pathfinding
  * **OPTIMIZATION** (`minor`): Reduce memory allocation in NavMeshGenerator#createNavigationPolygon()
  * **OPTIMIZATION** (`minor`): TerrainObstacleService: apply a rough simplification on self obstacles polygons
  * **OPTIMIZATION** (`medium`): Exclude very small objects from navigation mesh
  * **NEW FEATURE** (`minor`): Exclude fast moving objects as walkable face
  * **OPTIMIZATION** (`minor`): NavMeshGenerator#computePolytopeFootprint: put result in cache
  * **NEW FEATURE** (`medium`): Insert bevel planes during Polytope#buildExpanded* (see BrushExpander.cpp from Hesperus)
- Pathfinding
  * **OPTIMIZATION** (`medium`): When compute A* G score: avoid to execute funnel algorithm from start each time
  * **OPTIMIZATION** (`medium`): When search start and end triangles: use AABBox Tree algorithm
  * **NEW FEATURE** (`major`): Implement steering behaviour (see <https://gamedevelopment.tutsplus.com/tutorials/understanding-steering-behaviors-collision-avoidance--gamedev-7777>)
  * **NEW FEATURE** (`major`): AICharacterController should refresh path points each time the path request is updated 

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
  * **OPTIMIZATION** (`minor`): EPA, add new point on convex hull to improve thanks to adjacent edges: find faster the triangles visible by the new point (see Reactphysics3d code)
* Island
  * **BUG** (`major`): A body balancing from one side to the other side (e.g.: cone on his base) could be disabled when velocity reach zero
    * Tips 1: don't disable bodies when there is only one contact point
* Constraints solver
  * **BUG** (`medium`): A restitution of 1.0 introduce new force in the system. Example: a superball bounces higher and higher at each collision
* Character
  * **NEW FEATURE** (`medium`): Handle stair for character controller
  * **NEW FEATURE** (`medium`): Apply impulse on objects hit by character or falling on the character

# Sound engine
* Sound trigger
  * **NEW FEATURE** (`medium`): Support convex hull sound shape
  * **NEW FEATURE** (`major`): Support portal IN and portal OUT to trigger sound
* Listener    
  * **NEW FEATURE** (`major`): Define an orientation and a velocity to the listener

# Map editor
* Render
  * **NEW FEATURE** (`medium`): When change orientation of cylinder/capsule: resize physics shape to fit the object size
  * **NEW FEATURE** (`medium`): Make objects without physics selectable with left click
  * **NEW FEATURE** (`medium`): Display box around objects which don't have mesh (allow to select them easily with left click)
* UI
  * **NEW FEATURE** (`minor`): Allow undoing the actions
