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
    DeferredSecondPass --> AmbientOcclusionBlur(Ambient Occlusion - Horizontal/Vert Blur)
    AmbientOcclusionBlur --> DeferredFirstPass
    AmbientOcclusionBlur --> AmbientOcclusion(Ambient Occlusion)
    AmbientOcclusion --> DeferredFirstPass
```
