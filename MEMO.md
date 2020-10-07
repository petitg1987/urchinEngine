# Engine memo
- Coordinates:
    - 3D (e.g.: *3D models, Physics rigid bodies, 3D Nav mesh*)
        - X+ (left), X- (right)
        - Y+ (top), Y- (bottom)
        - Z+ (near), Z- (far)
    - 2D top view (e.g.: *2D Nav mesh*)
        - X+ (left), X- (right)
        - Y+ (far), Y- (near)

# Development tips
- Error handling:
    - Use exception (`throw std::runtime_error(...)`) for methods which could be wrongly used by the final user
    - Use assert (`assert(...)`) for methods which could be wrongly used by the engine developer
        - *Note:* surround assert with `#ifndef NDEBUG`/`#endif` when condition has bad performance
    - Use logger (`Logger::logger().logError(...)`) when the result of an algorithm is not the one excepted
        - *Note:* surround logger call with `if(Check::instance()->additionalChecksEnable()){`/`}` when condition has bad performance
- Verify when `new` operator is called:
    - Use following source code and add a debug point:
        ```
        void* operator new(std::size_t sz){
            return std::malloc(sz);
        }

        void* operator new[](std::size_t sz) {
            return std::malloc(sz);
        }
        ```

# Third party games
- Mesh
	- Call Of Duty 4: characters of 6000 triangles + normal mapping
	- Crysis 1: scene with 500 000 to 1 million of triangles displayed
	- Horizon Zero Dawn: trees of 10 000 triangles in LOD1
- Texel density
	- Crysis: average of 512 pixels/m for textures
- Shadow
	- Skyrim: shadow map resolution 4096\*4096 (high quality), 2048\*2048 (medium quality), 2 shadow maps: primary and secondary
	- GTA V: shadow map resolution 1024\*4096 (4 rendering). http://www.adriancourreges.com/blog/2015/11/02/gta-v-graphics-study/
	- Assassin Creed Syncidate: PCSS Ultra use VRAM > 400Mo
	- The Division & Watch Dog 2: use Hybrid Frustum Traced Shadows
- Anti aliasing
	- Assassin Creed Syndicate: MSAA4x+FXAA (VRAM=392Mo) or TXAA4x+FXAA (VRAM=410Mo)
	- Far Cry 5: SMAA or TAA

