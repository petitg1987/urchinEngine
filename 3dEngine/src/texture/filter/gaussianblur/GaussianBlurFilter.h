#ifndef URCHINENGINE_GAUSSIANBLURFILTER_H
#define URCHINENGINE_GAUSSIANBLURFILTER_H

#include <string>
#include <vector>
#include <map>

#include "texture/filter/TextureFilter.h"

namespace urchin {

    class GaussianBlurFilterBuilder;

    class GaussianBlurFilter : public TextureFilter {
        public:
            enum BlurDirection {
                VERTICAL,
                HORIZONTAL
            };

            struct GaussianBlurShaderConst {
                uint32_t numberLayer;
                uint32_t isVerticalBlur;
                uint32_t nbTextureFetch;
            };

            GaussianBlurFilter(const GaussianBlurFilterBuilder*, BlurDirection);

        private:
            std::string getShaderName() const override;
            void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&) override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;

            std::vector<float> computeWeights() const;
            std::vector<float> computeWeightsLinearSampling(const std::vector<float>&) const;
            std::vector<float> computeOffsetsLinearSampling(const std::vector<float>&, const std::vector<float>&) const;

            static const unsigned int TEXTURE_FETCH_SHADER_LIMIT;

            BlurDirection blurDirection;
            unsigned int blurSize;

            unsigned int nbTextureFetch;
            unsigned int textureSize;
            std::vector<float> weightsLinearSampling;
            std::vector<float> offsetsLinearSampling;
    };

}

#endif
