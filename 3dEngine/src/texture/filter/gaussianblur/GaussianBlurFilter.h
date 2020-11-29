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

            GaussianBlurFilter(const GaussianBlurFilterBuilder*, BlurDirection);

        private:
            std::string getShaderName() const override;
            void completeShaderTokens(std::map<std::string, std::string>&) const override;

            std::vector<float> computeWeights() const;
            std::vector<float> computeWeightsLinearSampling(const std::vector<float>&) const;
            std::vector<float> computeOffsetsLinearSampling(const std::vector<float>&, const std::vector<float>&) const;

            BlurDirection blurDirection;
            unsigned int blurSize;

            unsigned int nbTextureFetch;
            unsigned int textureSize;
            std::string weightsTab;
            std::string offsetsTab;
    };

}

#endif
