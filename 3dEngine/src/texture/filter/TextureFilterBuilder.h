#ifndef URCHINENGINE_TEXTUREFILTERBUILDER_H
#define URCHINENGINE_TEXTUREFILTERBUILDER_H

#include <string>
#include <memory>
#include <stdexcept>

#include "TextureFilter.h"
#include "graphic/texture/model/TextureType.h"
#include "texture/filter/downsample/DownSampleFilter.h"
#include "texture/filter/gaussianblur/GaussianBlurFilter.h"
#include "texture/filter/bilateralblur/BilateralBlurFilter.h"

namespace urchin {

    template<typename T> class TextureFilterBuilder {
        public:
            explicit TextureFilterBuilder(std::string, const std::shared_ptr<Texture>&);
            virtual ~TextureFilterBuilder() = default;

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
            std::string name;

            //source texture
            const std::shared_ptr<Texture> sourceTexture;

            //target texture
            unsigned int textureWidth, textureHeight;
            TextureType pTextureType;
            unsigned int pTextureNumberLayer;
            TextureFormat pTextureFormat;
    };

    #include "TextureFilterBuilder.inl"

}

#endif
