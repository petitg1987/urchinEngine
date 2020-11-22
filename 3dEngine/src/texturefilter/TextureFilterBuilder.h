#ifndef URCHINENGINE_TEXTUREFILTERBUILDER_H
#define URCHINENGINE_TEXTUREFILTERBUILDER_H

#include <GL/glew.h>
#include <string>
#include <memory>
#include <stdexcept>

#include "TextureFilter.h"
#include "graphic/texture/model/TextureType.h"
#include "texturefilter/downsample/DownSampleFilter.h"
#include "texturefilter/gaussianblur/GaussianBlurFilter.h"
#include "texturefilter/bilateralblur/BilateralBlurFilter.h"

namespace urchin {

    template<typename T> class TextureFilterBuilder {
        public:
            TextureFilterBuilder();
            virtual ~TextureFilterBuilder();

            T *textureSize(unsigned int, unsigned int);
            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            T *textureType(TextureType);
            TextureType getTextureType() const;

            T *textureNumberLayer(unsigned int);
            unsigned int getTextureNumberLayer() const;

            T *textureInternalFormat(int);
            int getTextureInternalFormat() const;

            T *textureFormat(unsigned int);
            unsigned int getTextureFormat() const;

            virtual std::unique_ptr<TextureFilter> build() = 0;

        private:
            T* _this();

            unsigned int textureWidth, textureHeight;

            TextureType pTextureType;
            unsigned int pTextureNumberLayer;
            int pTextureInternalFormat;
            unsigned int pTextureFormat;
    };

    #include "TextureFilterBuilder.inl"

}

#endif
