# Nav mesh generation algorithm
* __Input:__ `AIWorld` (updated by mapHandler)
* __Process__:
  - Build/update expanded polytopes for all `AIObject`
  - Resolve/retrieve walkable surface from expanded polytopes
  - For each walkable surface: create `NavPolygon`:
    - Get walkable surface: <br> ![](ws.png)
    - Find all obstacles of the walkable surface: <br> ![](obstacles.png)
    - Subtract obstacles on edge: <br> ![](subtract.png)
    - Triangulate with remaining obstacles: <br> ![](triang.png)
    - Add `NavPolygon` to `NavMesh`
