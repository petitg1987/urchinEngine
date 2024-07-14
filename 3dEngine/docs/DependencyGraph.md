# Dependency graph

```mermaid
graph BT
    Screen(Screen - Bloom Combine) --> Bloom(Bloom - Up/Down/Pre-Filter)
    Bloom --> Bloom
    Bloom --> Reflection(Reflection)
    Reflection --> AntiAliasing(Anti Aliasing)
    Screen --> Reflection
    AntiAliasing --> Transparent(Transparent)
    Transparent --> DefFSecondPass(Deferred Second Pass)
    DefFSecondPass --> ShadowMap(Shadow Map)
    DefFSecondPass --> DefFirstPass(Deferred First Pass - Albedo/Normal/Mat)
    Transparent --> DefFirstPass
    DefFSecondPass --> AmbientOcclusionHor(Ambient Occlusion - Horizontal Blur)
    AmbientOcclusionHor --> DefFirstPass
    AmbientOcclusionHor --> AmbientOcclusionVert(Ambient Occlusion - Vertical Blur)
    AmbientOcclusionVert --> DefFirstPass
    AmbientOcclusionVert --> AmbientOcclusion(Ambient Occlusion)
    AmbientOcclusion --> DefFirstPass
```
