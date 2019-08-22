# Nav mesh generation algorithm
* __Input:__ `AIWorld` (updated by mapHandler)
* __Process__:
  - Build/update expanded polytopes for all `AIObject`
  - For each walkable surface of expanded polytopes: create `NavPolygon`:
    - Get walkable surface: <br> ![](ws.png)
    - Find all obstacles of the walkable surface: <br> ![](obstacles.png)
    - Subtract obstacles on edge: <br> ![](subtract.png)
    - Triangulate with remaining obstacles: <br> ![](triang.png)
    - Add `NavPolygon` to `NavMesh`
