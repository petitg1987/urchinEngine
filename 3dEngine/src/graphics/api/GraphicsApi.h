#pragma once

//choose rendering graphics API
#define VULKAN_GRAPHICS_API

#ifdef VULKAN_GRAPHICS_API
    #include "graphics/api/vulkan/GraphicsApiService.h"
    #include "graphics/api/vulkan/render/GenericRenderer.h"
    #include "graphics/api/vulkan/render/GenericCompute.h"
    #include "graphics/api/vulkan/render/TextureCopier.h"
    #include "graphics/api/vulkan/render/target/RenderTarget.h"
    #include "graphics/api/vulkan/render/target/OffscreenRender.h"
    #include "graphics/api/vulkan/render/target/ScreenRender.h"
    #include "graphics/api/vulkan/render/shader/Shader.h"
    #include "graphics/api/vulkan/texture/Texture.h"
    #include "graphics/api/vulkan/texture/TextureParam.h"
#endif
