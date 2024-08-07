# Dependency graph

```mermaid
graph BT
    Screen(Screen - Bloom Combine) --> Bloom(Bloom - Up/Down/Pre-Filter)
    Bloom --> Bloom
    Bloom --> ReflectionCombine(Reflection Combine)
    ReflectionCombine --> AntiAliasing(Anti Aliasing)
    ReflectionCombine --> ReflectionColor(Reflection Color)
    ReflectionColor --> AntiAliasing(Anti Aliasing)
    ReflectionColor --> DeferredFirstPass(Deferred First Pass - Albedo/Normal/Mat)
    AntiAliasing --> Transparent(Transparent)
    Transparent --> DeferredSecondPass(Deferred Second Pass)
    DeferredSecondPass --> ShadowMap(Shadow Map)
    DeferredSecondPass --> DeferredFirstPass
    Transparent --> DeferredFirstPass
    DeferredSecondPass --> AmbientOcclusionHor(Ambient Occlusion - Horizontal Blur)
    AmbientOcclusionHor --> DeferredFirstPass
    AmbientOcclusionHor --> AmbientOcclusionVert(Ambient Occlusion - Vertical Blur)
    AmbientOcclusionVert --> DeferredFirstPass
    AmbientOcclusionVert --> AmbientOcclusion(Ambient Occlusion)
    AmbientOcclusion --> DeferredFirstPass
```
