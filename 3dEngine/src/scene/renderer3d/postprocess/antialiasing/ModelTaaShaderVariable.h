#pragma once

#include <UrchinCommon.h>

#include <scene/renderer3d/model/displayer/CustomModelShaderVariable.h>

namespace urchin {

    class ModelTaaShaderVariable final : public CustomModelShaderVariable {
        public:
            explicit ModelTaaShaderVariable();

            void onRenderingSceneResize(unsigned int, unsigned int);

            void setupMeshRenderer(const std::shared_ptr<GenericRendererBuilder>&, uint32_t, uint32_t) override;
            void loadCustomShaderVariables(GenericRenderer&, uint32_t, uint32_t) override;

        private:
            Matrix4<float> haltonSequenceX;
            Matrix4<float> haltonSequenceY;

            struct FrameData {
                alignas(4) int frameCount;
            } frameData;

            struct JitterData {
                alignas(16) Matrix4<float> offsetsX;
                alignas(16) Matrix4<float> offsetsY;
            } jitterData;
            bool jitterOffsetsUpdated;
    };

}
