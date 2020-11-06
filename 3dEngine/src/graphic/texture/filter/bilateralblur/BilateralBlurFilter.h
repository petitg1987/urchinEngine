#ifndef URCHINENGINE_BILATERALBLURFILTER_H
#define URCHINENGINE_BILATERALBLURFILTER_H

#include <string>
#include <map>

#include "graphic/texture/filter/TextureFilter.h"

namespace urchin {

    class BilateralBlurFilterBuilder;

    class BilateralBlurFilter : public TextureFilter {
        public:
            enum BlurDirection
            {
                VERTICAL,
                HORIZONTAL
            };

            BilateralBlurFilter(const BilateralBlurFilterBuilder *, BlurDirection);

            void onCameraProjectionUpdate(float, float);

        private:
            std::string getShaderName() const override;
            void initializeAdditionalUniforms(unsigned int) override;
            void addFurtherTextures(const std::shared_ptr<GenericDisplayer> &) const override;
            void completeShaderTokens(std::map<std::string, std::string> &) const override;

            std::vector<float> computeOffsets() const;

            BlurDirection blurDirection;
            unsigned int blurSize;
            float blurSharpness;
            unsigned int depthTextureID;

            unsigned int textureSize;
            int cameraPlanesLoc;

            std::string offsetsTab;
    };

}

#endif
