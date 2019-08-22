# Nav mesh generation algorithm
* __Input:__ `AIWorld` (updated by mapHandler)
* __Process__:
  - For each `AIObject` changed:
    - Create or update an expanded polytope based on agent size
    - Determine walkable polytope surfaces according to configuration and register them
  - For each registered walkable surface: create `NavPolygon`:
    - Walkable surface: <br> ![](ws.png)
    - Find all obstacles of the walkable surface: <br> ![](obstacles.png)
    - Subtract obstacles on edge: <br> ![](subtract.png)
    - Triangulate with remaining obstacles: <br> ![](triang.png)
    - Add `NavPolygon` to `NavMesh`
