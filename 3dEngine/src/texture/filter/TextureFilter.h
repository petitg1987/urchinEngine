#ifndef URCHINENGINE_TEXTUREFILTER_H
#define URCHINENGINE_TEXTUREFILTER_H

#include <string>
#include <memory>
#include <map>
#include <vector>

#include "graphic/shader/model/Shader.h"
#include "graphic/shader/model/ShaderVar.h"
#include "graphic/render/GenericRenderer.h"
#include "graphic/render/target/OffscreenRender.h"

namespace urchin {

    template<class T> class TextureFilterBuilder;

    class TextureFilter {
        public:
            template<class BUILDER> explicit TextureFilter(const TextureFilterBuilder<BUILDER> *);
            virtual ~TextureFilter() = default;

            void initialize();

            const std::shared_ptr<Texture> &getTexture() const;

            void applyOn(const std::shared_ptr<Texture> &, int layersToUpdate = -1) const;

        protected:
            virtual std::string getShaderName() const = 0;
            virtual void initiateAdditionalShaderVariables(const std::unique_ptr<Shader> &);
            virtual void addFurtherTextures(const std::unique_ptr<GenericRenderer> &) const;
            virtual void completeShaderTokens(std::map<std::string, std::string> &) const = 0;

            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            const std::unique_ptr<Shader> &getTextureFilterShader() const;

            std::string toShaderVectorValues(std::vector<float> &) const;

        private:
            void initializeDisplay();
            void initializeTexture();

            bool isInitialized;

            unsigned int textureWidth, textureHeight;
            TextureType textureType;
            unsigned int textureNumberLayer;
            TextureFormat textureFormat;

            std::unique_ptr<GenericRenderer> textureRenderer;
            std::unique_ptr<Shader> textureFilterShader;
            ShaderVar layersToUpdateShaderVar;

            std::unique_ptr<OffscreenRender> offscreenRenderTarget;
            std::shared_ptr<Texture> texture;
    };

    #include "TextureFilter.inl"

}

#endif
