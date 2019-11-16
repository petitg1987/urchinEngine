# Nav mesh generation algorithm
* __Input:__ `AIWorld` (updated by mapHandler)
* Create empty `NavMesh` object
* __NavMeshGenerator::updateExpandedPolytopes()__:
  * For each new or updated `AIObject` in scene:
    * Create or update a `NavObject` and add it in *navObjectsToRefresh*
    * Compute expanded polytope and determine walkable surfaces
* __NavMeshGenerator::updateNavObstacles()__:
  * For each *navObjectsToRefresh*
    * Determine the obstacles and add them in *newAffectedNavObjects*
  * For each *newAffectedNavObjects*
    * Determine the obstacles
  * Add all *newAffectedNavObjects* in *navObjectsToRefresh*
* __NavMeshGenerator::updateNavPolygons()__:
  * For each *navObjectsToRefresh* and each walkable surfaces:
    * Walkable surface: <br> ![](ws.png)
    * Find all obstacles of the walkable surface: <br> ![](obstacles.png)
    * Subtract obstacles on walkable surface outline: <br> ![](subtract.png)
    * Triangulate with remaining obstacles: <br> ![](triang.png)
* Add `NavPolygon` to `NavMesh`
