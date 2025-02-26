#pragma once

#include <graphics/api/vulkan/render/target/RenderTarget.h>
#include <graphics/api/vulkan/render/shader/Shader.h>
#include <graphics/api/vulkan/render/GenericRenderer.h>

namespace urchin {

    class OutputRenderer {
        public:
            OutputRenderer(RenderTarget&, float);
            ~OutputRenderer();

            void refreshInputTexture(const std::shared_ptr<Texture>&);
            void onGammaFactorUpdate(float);

            void render(unsigned int) const;

        private:
            struct OutputShaderConst {
                float gammaFactor;
            };

            void createOrUpdateRenderingObjects();
            void clearRenderingObjects();
            void createOrUpdateRenderer();
            void createOrUpdateShaders();

            static constexpr uint32_t SCENE_TEX_UNIFORM_BINDING = 0;

            RenderTarget& outputRenderTarget;
            float gammaFactor;
            std::shared_ptr<Texture> sceneTexture;

            std::unique_ptr<Shader> outputShader;
            std::unique_ptr<GenericRenderer> outputRenderer;
    };

}
