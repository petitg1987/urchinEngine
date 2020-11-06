#ifndef URCHINENGINE_TEXTUREFILTERBUILDER_H
#define URCHINENGINE_TEXTUREFILTERBUILDER_H

#include <GL/glew.h>
#include <string>
#include <memory>
#include <stdexcept>

#include "TextureFilter.h"
#include "graphic/texture/filter/downsample/DownSampleFilter.h"
#include "graphic/texture/filter/gaussianblur/GaussianBlurFilter.h"
#include "graphic/texture/filter/bilateralblur/BilateralBlurFilter.h"

namespace urchin {

    template<typename T> class TextureFilterBuilder {
        public:
            TextureFilterBuilder();
            virtual ~TextureFilterBuilder();

            T *textureSize(unsigned int, unsigned int);
            unsigned int getTextureWidth() const;
            unsigned int getTextureHeight() const;

            T *textureType(unsigned int);
            unsigned int getTextureType() const;

            T *textureNumberLayer(unsigned int);
            unsigned int getTextureNumberLayer() const;

            T *textureInternalFormat(int);
            int getTextureInternalFormat() const;

            T *textureFormat(unsigned int);
            unsigned int getTextureFormat() const;

            virtual std::shared_ptr<TextureFilter> build() = 0;

        private:
            T* _this();

            unsigned int textureWidth, textureHeight;

            unsigned int pTextureType;
            unsigned int pTextureNumberLayer;
            int pTextureInternalFormat;
            unsigned int pTextureFormat;
    };

    #include "TextureFilterBuilder.inl"

}

#endif
