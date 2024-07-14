#pragma once

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    class ReflectionApplier {
        public:
            explicit ReflectionApplier(bool);

            void refreshInputTexture(const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&, const std::shared_ptr<Texture>&);
            const std::shared_ptr<Texture>& getOutputTexture() const;

            void applyReflection(std::uint32_t, unsigned int) const;

        private:
            void clearRenderer();
            void createOrUpdateRenderer();
            void createOrUpdateShader();

            static constexpr uint32_t INPUT_TEX_UNIFORM_BINDING = 0;

            bool useNullRenderTarget;

            std::shared_ptr<Texture> depthTexture;
            std::shared_ptr<Texture> normalAndAmbientTexture;
            std::shared_ptr<Texture> materialTexture;
            std::shared_ptr<Texture> illuminatedTexture;
            std::shared_ptr<Texture> outputTexture;

            std::unique_ptr<RenderTarget> renderTarget;
            std::unique_ptr<Shader> shader;
            std::unique_ptr<GenericRenderer> renderer;
    };

}
