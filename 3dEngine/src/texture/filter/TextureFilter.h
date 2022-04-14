#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>

#include <api/render/shader/model/Shader.h>
#include <api/render/GenericRenderer.h>
#include <api/render/target/OffscreenRender.h>

namespace urchin {

    template<class T> class TextureFilterBuilder;

    class TextureFilter {
        public:
            template<class BUILDER> explicit TextureFilter(const TextureFilterBuilder<BUILDER>*);
            virtual ~TextureFilter();

            void initialize();

            const std::shared_ptr<Texture>& getTexture() const;

            void applyFilter(std::uint64_t, unsigned int) const;

        protected:
            virtual std::string getShaderName() const = 0;
            virtual void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) = 0;
            virtual std::unique_ptr<ShaderConstants> buildShaderConstants() const = 0;

            TextureFormat getTextureFormat() const;
            TextureType getTextureType() const;
            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;
            unsigned int getTextureLayer() const;

            const Shader& getTextureFilterShader() const;
            GenericRenderer& getTextureRenderer() const;

        private:
            void initializeTexture();
            void initializeDisplay();

            static constexpr unsigned int TEXTURE_LAYER_SHADER_LIMIT = 10; //must be equals to 'TEXTURE_LAYER_SHADER_LIMIT' in texFilter shaders
            bool isInitialized;
            bool useNullRenderTarget;
            std::string name;

            //source texture
            std::shared_ptr<Texture> sourceTexture;

            //target texture
            unsigned int textureWidth;
            unsigned int textureHeight;
            TextureType textureType;
            unsigned int textureNumberLayer;
            TextureFormat textureFormat;

            std::unique_ptr<RenderTarget> renderTarget;
            std::shared_ptr<Texture> texture;

            std::unique_ptr<GenericRenderer> textureRenderer;
            std::unique_ptr<Shader> textureFilterShader;
    };

    #include "TextureFilter.inl"

}
