# Nav mesh generation algorithm
* __Input:__ `AIWorld` (updated by mapHandler)
* __Process__:
  - Create empty `NavMesh` object
  - For each `AIObject` changed in scene:
    - Create or update an expanded polytope based on agent size
    - Determine walkable polytope surfaces and register them
  - For each registered walkable surface: create `NavPolygon`:
    - Walkable surface: <br> ![](ws.png)
    - Find all obstacles of the walkable surface: <br> ![](obstacles.png)
    - Subtract obstacles on walkable surface outline: <br> ![](subtract.png)
    - Triangulate with remaining obstacles: <br> ![](triang.png)
    - Add `NavPolygon` to `NavMesh`
  - Return the `NavMesh` object
