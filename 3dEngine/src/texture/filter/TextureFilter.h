#ifndef URCHINENGINE_TEXTUREFILTER_H
#define URCHINENGINE_TEXTUREFILTER_H

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "graphic/render/shader/model/Shader.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/OffscreenRender.h"

namespace urchin {

    template<class T> class TextureFilterBuilder;

    class TextureFilter {
        public:
            template<class BUILDER> explicit TextureFilter(const TextureFilterBuilder<BUILDER>*);
            virtual ~TextureFilter() = default;

            void initialize();

            const std::shared_ptr<Texture>& getTexture() const;

            void applyFilter(int layersToUpdate = -1) const;

        protected:
            virtual std::string getShaderName() const = 0;
            virtual void initiateAdditionalDisplay(const std::unique_ptr<GenericRendererBuilder>&);
            virtual void completeShaderTokens(std::map<std::string, std::string>&) const = 0;

            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            const std::shared_ptr<Shader>& getTextureFilterShader() const;
            const std::unique_ptr<GenericRenderer>& getTextureRenderer() const;

            static std::string toShaderVectorValues(std::vector<float>&);

        private:
            void initializeTexture();
            void initializeDisplay();

            bool isInitialized;

            //source texture
            std::shared_ptr<Texture> sourceTexture;

            //target texture
            unsigned int textureWidth, textureHeight;
            TextureType textureType;
            unsigned int textureNumberLayer;
            TextureFormat textureFormat;

            std::shared_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> texture;

            std::unique_ptr<GenericRenderer> textureRenderer;
            std::shared_ptr<Shader> textureFilterShader;
    };

    #include "TextureFilter.inl"

}

#endif
