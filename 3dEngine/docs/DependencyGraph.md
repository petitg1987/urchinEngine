# Dependency graph

```mermaid
graph BT
    A(Screen - Bloom Combine) --> B(Bloom - Up/Down/Pre-Filter)
    B --> B
    B --> C(Anti Aliasing)
    A --> C
    C --> D(Transparent)
    D --> E(Deferred Second Pass)
    E --> H(Shadow Map)
    E --> K(Deferred First Pass - Albedo/Normal/Mat)
    D --> K
    E --> M(Ambient Occlusion - Horizontal Bilateral)
    M --> K
    M --> N(Ambient Occlusion - Vertical Bilateral)
    N --> K
    N --> O(Ambient Occlusion)
    O --> K
```
