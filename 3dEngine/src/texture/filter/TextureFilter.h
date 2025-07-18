#pragma once

#include <string>
#include <memory>

#include <graphics/api/GraphicsApi.h>

namespace urchin {

    template<class T> class TextureFilterBuilder;

    class TextureFilter {
        public:
            template<class BUILDER> explicit TextureFilter(const TextureFilterBuilder<BUILDER>*);
            virtual ~TextureFilter() = default;

            void initialize();

            const std::shared_ptr<Texture>& getTexture() const;

            void applyFilter(uint32_t, unsigned int) const;

        protected:
            virtual std::string getShaderName() const = 0;
            virtual void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) = 0;
            virtual std::unique_ptr<ShaderConstants> buildShaderConstants() const = 0;

            void createOrUpdateRenderer();

            TextureFormat getTextureFormat() const;
            TextureType getTextureType() const;
            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

        private:
            void initializeTexture();
            void initializeDisplay();

            bool isInitialized;
            bool isTestMode;
            std::string name;

            //source texture
            std::shared_ptr<Texture> sourceTexture;

            //target texture
            unsigned int textureWidth;
            unsigned int textureHeight;
            TextureType textureType;
            TextureFormat textureFormat;

            std::unique_ptr<OffscreenRender> renderTarget;
            std::shared_ptr<Texture> texture;

            std::unique_ptr<GenericRenderer> textureRenderer;
            std::unique_ptr<Shader> textureFilterShader;
    };

    #include "TextureFilter.inl"

}
