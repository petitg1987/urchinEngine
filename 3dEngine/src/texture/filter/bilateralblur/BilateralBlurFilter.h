#ifndef URCHINENGINE_BILATERALBLURFILTER_H
#define URCHINENGINE_BILATERALBLURFILTER_H

#include <string>
#include <map>

#include "texture/filter/TextureFilter.h"

namespace urchin {

    class BilateralBlurFilterBuilder;

    class BilateralBlurFilter : public TextureFilter {
        public:
            enum BlurDirection {
                VERTICAL,
                HORIZONTAL
            };

            BilateralBlurFilter(const BilateralBlurFilterBuilder*, BlurDirection);

            void onCameraProjectionUpdate(float, float);

        private:
            std::string getShaderName() const override;
            void initiateAdditionalDisplay(const std::unique_ptr<GenericRendererBuilder>&) override;
            void completeShaderTokens(std::map<std::string, std::string>&) const override;

            std::vector<float> computeOffsets() const;

            //source texture
            std::shared_ptr<Texture> depthTexture;

            BlurDirection blurDirection;
            unsigned int blurSize;
            float blurSharpness;

            unsigned int textureSize;
            struct {
                alignas(4) float nearPlane;
                alignas(4) float farPlane;
            } cameraPlanes;

            std::string offsetsTab;
    };

}

#endif
