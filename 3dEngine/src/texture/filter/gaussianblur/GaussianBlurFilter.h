#pragma once

#include <string>
#include <vector>
#include <map>

#include <texture/filter/TextureFilter.h>

namespace urchin {

    class GaussianBlurFilterBuilder;

    class GaussianBlurFilter final : public TextureFilter {
        public:
            enum BlurDirection {
                VERTICAL,
                HORIZONTAL
            };

            GaussianBlurFilter(const GaussianBlurFilterBuilder*, BlurDirection);

        private:
            struct GaussianBlurShaderConst {
                uint32_t isVerticalBlur;
                uint32_t nbTextureFetch;
            };

            std::string getShaderName() const override;
            void completeRenderer(const std::shared_ptr<GenericRendererBuilder>&, const std::shared_ptr<TextureReader>&) override;
            std::unique_ptr<ShaderConstants> buildShaderConstants() const override;

            std::vector<float> computeWeights() const;
            std::vector<float> computeWeightsLinearSampling(const std::vector<float>&) const;
            std::vector<float> computeOffsetsLinearSampling(const std::vector<float>&, const std::vector<float>&) const;

            static constexpr unsigned int TEXTURE_FETCH_SHADER_LIMIT = 9; //must be equals to 'KERNEL_RADIUS' in texFilterGaussianBlurVec2Array shader

            static constexpr uint32_t OFFSETS_DATA_UNIFORM_BINDING = 0;
            static constexpr uint32_t WEIGHTS_DATA_UNIFORM_BINDING = 1;
            static constexpr uint32_t SRC_TEX_UNIFORM_BINDING = 2;

            BlurDirection blurDirection;
            unsigned int blurSize;

            unsigned int nbTextureFetch;
            unsigned int textureSize;
            std::vector<float> weightsLinearSampling;
            std::vector<float> offsetsLinearSampling;
    };

}
