#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#include <texture/filter/TextureFilter.h>
#include <texture/filter/gaussianblur/GaussianBlurFilter.h>
#include <texture/filter/bilateralblur/BilateralBlurFilter.h>
#include <api/texture/TextureType.h>

namespace urchin {

    template<typename T> class TextureFilterBuilder {
        public:
            TextureFilterBuilder(bool, std::string, const std::shared_ptr<Texture>&);
            virtual ~TextureFilterBuilder() = default;

            bool useNullRenderTarget() const;
            const std::string& getName() const;

            const std::shared_ptr<Texture>& getSourceTexture() const;

            T* textureSize(unsigned int, unsigned int);
            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            T* textureType(TextureType);
            TextureType getTextureType() const;

            T* textureNumberLayer(unsigned int);
            unsigned int getTextureNumberLayer() const;

            T* textureFormat(TextureFormat);
            TextureFormat getTextureFormat() const;

            virtual std::unique_ptr<TextureFilter> build() = 0;

        private:
            T* _this();

            //general
            bool bUseNullRenderTarget;
            std::string name;

            //source texture
            const std::shared_ptr<Texture> sourceTexture;

            //target texture
            unsigned int textureWidth;
            unsigned int textureHeight;
            TextureType pTextureType;
            unsigned int pTextureNumberLayer;
            TextureFormat pTextureFormat;
    };

    #include "TextureFilterBuilder.inl"

}
